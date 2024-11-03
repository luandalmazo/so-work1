/* Created by: Luan Matheus Trindade Dalmazo [lmtd21] and Mateus de Oliveira Silva [mos20] */

#ifndef FIFO_QUEUE_H
#define FIFO_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "../include/fifo_queue.h"

#define MAX_QUEUE_SIZE 4096

typedef struct fifoQ_s {
    sem_t mutex;
    int front;
    int rear;
    int waiting_count;
    sem_t wait_sems[MAX_QUEUE_SIZE]; // Array of semaphores, one per thread in the queue
} FifoQT;

void init_fifoQ(FifoQT *F);
void espera(FifoQT *F);
void liberaPrimeiro(FifoQT *F);

#endif
