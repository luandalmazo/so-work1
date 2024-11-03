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

char* get_current_time() {
    // Static buffer to hold the formatted time string
    static char time_str[9];
    
    // Get the current time
    time_t now = time(NULL);
    
    // Format the time as a string with only hours, minutes, and seconds
    strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));

    return time_str;
}

void fifo_recurse_usage(FifoQT *fifo, int uso) {
    for (int uso = 0; uso < 3; uso++)
    {

        /* (A) simula executar algo no (prologo) */
        /* random sleep between: 0, 1, 2, 3*/
        int sleep_time = rand () % 4;
        fprintf(stdout, "Processo: %d Prologo: %d de %d segundos: %s \n", getpid(), uso, sleep_time, get_current_time());
        sleep(sleep_time);
        espera(fifo);

        /* (B) simula usar o recurso com exclusividade */
        /* random sleep between: 0, 1, 2, 3*/
        sleep_time = rand() % 4;
        fprintf(stdout, "Processo: %d USO: %d por %d segundos: %s \n", getpid(), uso, sleep_time, get_current_time());
        sleep(sleep_time);

        liberaPrimeiro(fifo);

        /* (C) simula executar algo (epilogo)  */
        /* random sleep between: 0, 1, 2, 3*/
        sleep_time = rand() % 4;
        fprintf(stdout, "Processo: %d Epilogo: %d de %d segundos: %s \n", getpid(), uso, sleep_time, get_current_time());
        sleep(sleep_time);
    }
    printf("Processo %d terminando\n", getpid());
}

void sleep_random_time(int num_processes, int nProc)
{
    srand(time(NULL) + getpid());
    int sleep_time = rand() % num_processes;
    printf("Processo: %d [Numero logico: %d] iniciando com sleep de %d segundos: %s\n", getpid(), nProc, sleep_time, get_current_time());
    sleep(sleep_time);
}

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
    int nProc;

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
        nProc = i + 1;
        if (pid == 0)
        {   
            /* ONLY CHILD PROCESS ENTERS HERE */
            printf("Processo criado. PID: %d, PID-PAI: %d, Número lógico %d\n", getpid(), getppid(), nProc);

            sleep_random_time(num_processes, nProc);

            /* synchronize processes */
            process_barrier(barr, nProc);

            fifo_recurse_usage(fifo, 3);

            exit(0);
        }
    }

    /* ONLY PARENT PROCESS ENTERS HERE */
    nProc = 0;
    process_barrier(barr, nProc);
    sleep_random_time(num_processes, nProc);

    fifo_recurse_usage(fifo, 3);
    
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
    //sem_destroy(&fifo->mutex);
    //sem_destroy(&fifo->wait_sem);

    /* detaching shared memory */
    shmdt(barr);
    shmdt(fifo);

    /* destroying shared memory */
    shmctl(shared_memory_id, IPC_RMID, NULL);

    return 0;
}