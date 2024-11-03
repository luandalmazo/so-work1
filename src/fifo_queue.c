#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "../include/fifo_queue.h"


void init_fifoQ(FifoQT *F);
void espera(FifoQT *F);
void liberaPrimeiro(FifoQT *F);
int is_empty(FifoQT *F);
void lock_queue_struct(FifoQT *F);
void unlock_queue_struct(FifoQT *F);
void enqueue(FifoQT *F);
sem_t* dequeue(FifoQT *F);
sem_t* get_first(FifoQT *F);

// Initialize the FIFO queue
void init_fifoQ(FifoQT *F) {
    sem_init(&F->mutex, 1, 1);
    F->front = 0;
    F->rear = 0;
    F->waiting_count = 0;

    // Initialize all semaphores in the array
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        sem_init(&F->wait_sems[i], 1, 0); // Initialize to 0
    }
}

// Waits for an element in the FIFO queue
void espera(FifoQT *F) {
    lock_queue_struct(F);
    
    enqueue(F); // Enqueue this thread's position
    F->waiting_count++;
    
    unlock_queue_struct(F);

    // Wait on this thread's specific semaphore
    if (F->waiting_count > 1)
    {
        sem_wait(&F->wait_sems[F->rear - 1]); // Wait on the semaphore corresponding to this thread
    }
}

// Frees the first element in the FIFO queue
void liberaPrimeiro(FifoQT *F) {
    lock_queue_struct(F);

    dequeue(F); // Remove the semaphore from the queue
    if (!is_empty(F)) {
        sem_post(&F->wait_sems[F->front]); // Signal the semaphore for the first thread in line
    }

    unlock_queue_struct(F);
}

// Check if the queue is empty
int is_empty(FifoQT *F) {
    return F->waiting_count == 0;
}

// Lock the queue structure
void lock_queue_struct(FifoQT *F) {
    sem_wait(&F->mutex);
}

// Unlock the queue structure
void unlock_queue_struct(FifoQT *F) {
    sem_post(&F->mutex);
}

// Enqueue a semaphore into the FIFO queue
void enqueue(FifoQT *F) {
    F->rear = (F->rear + 1) % MAX_QUEUE_SIZE; // Update rear index
}

// Dequeue a semaphore from the FIFO queue
sem_t* dequeue(FifoQT *F) {
    sem_t *sem = &F->wait_sems[F->front]; // Get the semaphore at the front
    F->front = (F->front + 1) % MAX_QUEUE_SIZE; // Update front index
    F->waiting_count--; // Decrease the waiting count
    return sem;
}

