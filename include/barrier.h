/* Created by: Luan Matheus Trindade Dalmazo [lmtd21] and Mateus de Oliveira Silva [mos20] */

#ifndef BARRIER_H
#define BARRIER_H
#define OK 0
#include <semaphore.h>

/* Barrier struct */
typedef struct barrier_s {
    int total_processes;
    int waiting_processes;
    sem_t mutex;
    sem_t barrier_semaphore;
} barrier_t;

/* Barrier initialization */
void init_barrier(barrier_t *barr, int n);

/* Synchronizes processes */
void process_barrier(barrier_t *barr, int nProc);

#endif