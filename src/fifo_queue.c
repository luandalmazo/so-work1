#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "../include/fifo_queue.h"

int is_empty(FifoQT *F); 
void lock_queue(FifoQT *F);
void unlock_queue(FifoQT *F);
void remove_first_from_queue(FifoQT *F); 
void wait_in_queue(FifoQT *F); 

/**
 * @brief Initializes the FIFO queue.
 *
 * This function sets up the FIFO queue structure pointed to by `F`.
 *
 * @param F Pointer to the FIFO queue structure to be initialized.
 */
void init_fifoQ(FifoQT *F) {
    // Inicializa os semáforos
    sem_init(&F->mutex, 1, 1);          // Mutex inicializado para 1
    sem_init(&F->wait_sem, 1, 0);       // Semáforo de espera inicializado para 0
    F->waiting_count = 0;                // Contador de processos esperando
}

/**
 * @brief Waits for an element in the FIFO queue.
 *
 * This function blocks the calling thread until an element is available
 * in the FIFO queue. It is typically used in a producer-consumer scenario
 * where the consumer needs to wait for the producer to add elements to the queue.
 *
 * @param F A pointer to the FIFO queue structure.
 */
void espera(FifoQT *F) {
    lock_queue(F);

    if(is_empty(F)) {             // Se não houver processos esperando
        unlock_queue(F);
        return;
    }

    F->waiting_count++;                 // Incrementa o contador de processos esperando
    unlock_queue(F);
    wait_in_queue(F);
}

/**
 * @brief Frees the first element in the FIFO queue.
 *
 * This function removes and frees the memory of the first element in the 
 * FIFO queue pointed to by the given FifoQT pointer.
 *
 * @param F Pointer to the FIFO queue (FifoQT) from which the first element 
 *          will be freed.
 */
void liberaPrimeiro(FifoQT *F) {
    lock_queue(F);
    if (is_empty(F)) {            
        remove_first_from_queue(F);
    }
    unlock_queue(F);                 
}


/* ----------------------------- Helper Functions ----------------------------- */ 

int is_empty(FifoQT *F) {
    return F->waiting_count == 0;
}

void lock_queue(FifoQT *F) {
    sem_wait(&F->mutex);
}

void unlock_queue(FifoQT *F) {
    sem_post(&F->mutex);
}

void remove_first_from_queue(FifoQT *F) {
    F->waiting_count--;
    sem_post(&F->wait_sem);
}

void wait_in_queue(FifoQT *F) {
    sem_wait(&F->wait_sem);
}