/* Created by: Luan Matheus Trindade Dalmazo [lmtd21] and Mateus de Oliveira Silva [mos20] */

#include "../include/barrier.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
    
/**
 * @brief Initializes a barrier.
 *
 * This function sets up a barrier for synchronization purposes. The barrier
 * will be initialized to allow `n` threads to synchronize.
 *
 * @param barr Pointer to the barrier structure to be initialized.
 * @param n The number of threads that must call wait on the barrier before any
 *          of them successfully return from the wait.
 */
void init_barrier(barrier_t *barr, int n){
    barr->total_processes = n;
    barr->waiting_processes = 0;
    
    /* initializing mutex */
    /* pshared = 1 -> semaphore is shared between processes */
    /* value = 1 -> semaphore is unlocked */
    if (sem_init(&barr->mutex, 1, 1) != OK){
        perror("Error initializing mutex");
        exit(1);
    }

    /* initializing barrier semaphore */
    /* pshared = 1 -> semaphore is shared between processes */
    /* value = 0 -> semaphore is locked */
    if (sem_init(&barr->barrier_semaphore, 1, 0) != OK){
        perror("Error initializing barrier semaphore");
        exit(1);
    }

}

/**
 * @brief Processes the given barrier.
 *
 * This function performs operations on the provided barrier object.
 *
 * @param barr A pointer to the barrier object to be processed.
 */
void process_barrier(barrier_t *barr){

    /* locking the mutex */
    sem_wait(&barr->mutex);

    barr->waiting_processes++;  

    /* if all processes have arrived, unlock the barrier semaphore */
    if (barr->waiting_processes == barr->total_processes){
        for (int i = 0; i < barr->total_processes; i++){
            sem_post(&barr->barrier_semaphore);
        }
        barr->waiting_processes = 0;
    }

    /* unlocking the mutex */
    sem_post(&barr->mutex);

    /* waiting for all processes to arrive */
    sem_wait(&barr->barrier_semaphore);
}
