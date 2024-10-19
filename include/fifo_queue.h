#ifndef FIFO_QUEUE_H
#define FIFO_QUEUE_H

#include <semaphore.h>

// Structure for the FIFO queue of processes
typedef struct fifoQ_s {
    sem_t mutex;          // Semaphore to control access to the queue
    sem_t wait_sem;       // Semaphore to manage processes in the queue
    int waiting_count;    // Counter for the number of processes waiting in the queue
} FifoQT;

void init_fifoQ(FifoQT *F);

void espera(FifoQT *F);

void liberaPrimeiro(FifoQT *F);

#endif
