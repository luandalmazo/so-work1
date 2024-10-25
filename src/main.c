/* Created by: Luan Matheus Trindade Dalmazo [lmtd21] and Mateus de Oliveira Silva [mos20] */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include "../include/barrier.h"
#include "../include/fifo_queue.h"

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Número de argumentos inválido\n");
        printf("Uso: ./main <número de processos>\n");
        exit(1);
    }

    int num_processes = atoi(argv[1]);

    if (num_processes < 1)
    {
        printf("Número de processos inválido: %d\n", num_processes);
        exit(1);
    }

    int shared_memory_id;
    barrier_t *barr;
    FifoQT *fifo;

    /* creating the shared memory for the barrier */
    shared_memory_id = shmget(IPC_PRIVATE, sizeof(barrier_t), IPC_CREAT | 0666);
    if (shared_memory_id < OK)
    {
        perror("Error creating shared memory");
        exit(1);
    }

    /* attaching the shared memory to the father process */
    barr = (barrier_t *)shmat(shared_memory_id, NULL, 0);

    if (barr == (barrier_t *)-1)
    {
        perror("Error attaching shared memory");
        exit(1);
    }

    /* initializing the barrier */
    init_barrier(barr, num_processes + 1);

    /* creating the shared memory for the FIFO queue */
    shared_memory_id = shmget(IPC_PRIVATE, sizeof(FifoQT), IPC_CREAT | 0666);
    if (shared_memory_id < 0)
    {
        perror("Error creating shared memory");
        exit(1);
    }

    /* attaching the shared memory to the parent process */
    fifo = (FifoQT *)shmat(shared_memory_id, NULL, 0);

    if (fifo == (FifoQT *)-1)
    {
        perror("Error attaching shared memory");
        exit(1);
    }

    /* initializing the FIFO queue */
    init_fifoQ(fifo);

    /* creating num_processes children */
    for (int i = 0; i < num_processes; i++)
    {

        pid_t pid = fork();
        if (pid == 0)
        {
            /* generate a random seed for the child process */
            srand(time(NULL) + getpid());
            int sleep_time = rand() % num_processes;

            sleep(sleep_time);

            /* synchronize processes */
            process_barrier(barr);

            for (int uso = 0; uso < 3; uso++)
            {

                /* (A) simula executar algo no (prologo) */
                /* random sleep between: 0, 1, 2, 3*/
                int sleep_time = rand () % 4;
                printf("Processo: %d Prologo: %d de %d segundos\n", getpid(), uso, sleep_time);
                sleep(sleep_time);
                espera(fifo);

                /* (B) simula usar o recurso com exclusividade */
                /* random sleep between: 0, 1, 2, 3*/
                sleep_time = rand() % 4;
                printf("Processo: %d USO: %d por %d segundos\n", getpid(), uso, sleep_time);
                sleep(sleep_time);

                liberaPrimeiro(fifo);

                /* (C) simula executar algo (epilogo)  */
                /* random sleep between: 0, 1, 2, 3*/
                sleep_time = rand() % 4;
                printf("Processo: %d Epilogo: %d de %d segundos\n", getpid(), uso, sleep_time);
                sleep(sleep_time);
            }
            printf("Processo %d terminando\n", getpid());
            exit(0);
        }
    }

    /* father process */
    process_barrier(barr);
    srand(time(NULL));

    for (int uso = 0; uso < 3; uso++)
    {

        /* (A) simula executar algo no (prologo) */
        /* random sleep between: 0, 1, 2, 3*/
        int sleep_time = rand() % 4;
        printf("Processo: %d Prologo: %d de %d segundos\n", getpid(), uso, sleep_time);
        sleep(sleep_time);
        espera(fifo);

        /* (B) simula usar o recurso com exclusividade */
        /* random sleep between: 0, 1, 2, 3*/
        sleep_time = rand() % 4;
        printf("Processo: %d USO: %d por %d segundos\n", getpid(), uso, sleep_time);
        sleep(sleep_time);

        liberaPrimeiro(fifo);

        /* (C) simula executar algo (epilogo)  */
        /* random sleep between: 0, 1, 2, 3*/
        sleep_time = rand() % 4;
        printf("Processo: %d Epilogo: %d de %d segundos\n", getpid(), uso, sleep_time);
        sleep(sleep_time);
    }
    
    
    /* waiting for the children */
    for (int i = 0; i < num_processes; i++)
    {
        wait(NULL);
    }

    printf("Processo %d [pai] terminando\n", getpid());
    /* destroying semaphore for barrier */
    sem_destroy(&barr->mutex);
    sem_destroy(&barr->barrier_semaphore);
    /* destroying semephafore for FIFO*/
    sem_destroy(&fifo->mutex);
    sem_destroy(&fifo->wait_sem);

    /* detaching shared memory */
    shmdt(barr);
    shmdt(fifo);

    /* destroying shared memory */
    shmctl(shared_memory_id, IPC_RMID, NULL);

    return 0;
}