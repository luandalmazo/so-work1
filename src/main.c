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

void test_barrier(int num_processes){
    int shared_memory_id;
    barrier_t *barr;
    printf("\nTesting barrier\n");

    /* creating the shared memory for the barrier */
    /* shmget -> creates a new shared memory segment or locates an existing one */
    /* IPC_PRIVATE -> key for the shared memory */
    /* sizeof(barrier_t) -> size of the shared memory */
    /* IPC_CREAT | 0666 -> flags for the shared memory */
    shared_memory_id = shmget(IPC_PRIVATE, sizeof(barrier_t), IPC_CREAT | 0666);
    if (shared_memory_id < OK){
        perror("Error creating shared memory");
        exit(1);
    }

    /* attaching the shared memory to the father process */
    /* shmat -> attaches the shared memory segment identified by shared_memory_id to the address space of the calling process */
    /* 0 -> read and write access */
    barr = (barrier_t *) shmat(shared_memory_id, NULL, 0);
    if (barr == (barrier_t *) -1){
        perror("Error attaching shared memory");
        exit(1);
    }

    /* initializing the barrier */
    init_barrier(barr, num_processes+1);

    /* creating num_processes children */
    for (int i = 0; i < num_processes; i++){
        
        pid_t pid = fork();
        if (pid == 0){
            
            /* generate a random seed for the child process */
            srand(time(NULL) + getpid());
            int sleep_time = rand() % num_processes;
             
            /* print info and sleep */
            printf("Filho: PID = %d, PPID = %d, dormindo por %d segundos\n", getpid(), getppid(), sleep_time);
            sleep(sleep_time);

            /* synchronize processes */
            process_barrier(barr);

            printf("Filho: PID = %d, saindo da barreira\n", getpid());
            exit(0);
            }
        }

    /* father process */
    srand(time(NULL));
    int sleep_time = rand() % num_processes;
    
    printf("Pai: PID = %d, nProc = 0, dormindo por %d segundos\n", getpid(), sleep_time);
    sleep(sleep_time);

    /* synchronize processes */
    process_barrier(barr);

    printf("Pai: nProc = 0, PID = %d, saindo da barreira\n", getpid());

    /* waiting for the children */
    for (int i = 0; i < num_processes - 1; i++){
        wait(NULL);
    }

    /* destroying semaphore */
    sem_destroy(&barr->mutex);
    sem_destroy(&barr->barrier_semaphore);

    /* detaching shared memory */
    shmdt(barr);

    /* destroying shared memory */
    shmctl(shared_memory_id, IPC_RMID, NULL);
}

void test_fifo(int num_processes) {
    int shared_memory_id;
    FifoQT *fifo;

    printf("\nTesting FIFO queue\n");

    // Creating the shared memory for the FIFO queue
    shared_memory_id = shmget(IPC_PRIVATE, sizeof(FifoQT), IPC_CREAT | 0666);
    if (shared_memory_id < 0) {
        perror("Error creating shared memory");
        exit(1);
    }

    // Attaching the shared memory to the parent process
    fifo = (FifoQT *)shmat(shared_memory_id, NULL, 0);
    if (fifo == (FifoQT *)-1) {
        perror("Error attaching shared memory");
        exit(1);
    }

    // Initializing the FIFO queue
    init_fifoQ(fifo);

    // Creating num_processes children
    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == 0) { // Child process
            // Generate a random seed for the child process
            srand(time(NULL) + getpid());
            int sleep_time = rand() % num_processes + 1;

            // Print info and sleep
            printf("Filho: PID = %d, PPID = %d, dormindo por %d segundos\n", getpid(), getppid(), sleep_time);
            sleep(sleep_time);

            // Enqueue the process in the FIFO queue
            espera(fifo);
            printf("Filho: PID = %d, saindo da barreira\n", getpid());


            liberaPrimeiro(fifo); // Release the process from the FIFO queue
            // Simulate doing some work after exiting the queue
            sleep(1); // Simulating work after exiting the FIFO
            exit(0);
        }

    }

    // Parent process
    srand(time(NULL));
    int sleep_time = rand() % num_processes + 1;
    printf("Pai: PID = %d, nProc = 0, dormindo por %d segundos\n", getpid(), sleep_time);
    sleep(sleep_time);

    // Enqueue the parent in the FIFO queue
    espera(fifo);
    printf("Pai: nProc = 0, PID = %d, saindo da barreira\n", getpid());

    liberaPrimeiro(fifo); // Release the parent from the FIFO queue
    // Waiting for the children
    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }

    // Destroying semaphores
    sem_destroy(&fifo->mutex);
    sem_destroy(&fifo->wait_sem);

    // Detaching shared memory
    shmdt(fifo);

    // Destroying shared memory
    shmctl(shared_memory_id, IPC_RMID, NULL);
}


int main(int argc, char *argv[]){

    if(argc != 2){
        printf("Número de argumentos inválido\n");
        printf("Uso: ./main <número de processos>\n");
        exit(1);
    }

    int num_processes = atoi(argv[1]);

    if(num_processes < 1){
        printf("Número de processos inválido: %d\n", num_processes);
        exit(1);
    }

    test_barrier(num_processes);

    test_fifo(num_processes);

    return 0;

}