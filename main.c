/* Created by: Luan Matheus Trindade Dalmazo [lmtd21] and Mateus de Oliveira Silva [mos20] */

#include "barrier.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>


int main(int argc, char *argv[]){

    int num_processes = atoi(argv[1]);
    int shared_memory_id;
    barrier_t *barr;

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

    return 0;

}