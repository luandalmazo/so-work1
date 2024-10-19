# Process Synchronization with Barrier and FIFO Queue

## Overview

This program demonstrates process synchronization using two mechanisms: a barrier and a FIFO (First-In-First-Out) queue. It creates multiple child processes that synchronize their execution at a barrier and manage their order of execution with a FIFO queue, using shared memory and semaphores.

## Authors

- Luan Matheus Trindade Dalmazo [lmtd21]
- Mateus de Oliveira Silva [mos20]

## Code Structure

### Functions

- `test_barrier(int num_processes)`: 
  - Creates a barrier in shared memory.
  - Initializes the barrier and creates child processes that synchronize at the barrier.
  
- `test_fifo(int num_processes)`: 
  - Creates a FIFO queue in shared memory.
  - Initializes the FIFO queue and creates child processes that enqueue and dequeue themselves in the FIFO queue.

### Main Function

- The `main` function takes a command-line argument to specify the number of child processes to create.
- It validates the input and calls `test_barrier` followed by `test_fifo`.

## Usage

To compile and run the program, follow these steps:


1. Compile the program using the provided `Makefile`:
    ```bash
    make run <number_of_processes>
    ``` 
    or

    ```bash
    ./bin/main <number_of_processes>
    ```

## Project Structure

```
.
├── src
│   ├── main.c          # Entry point of the program
│   ├── other_source.c  # Other C source files
├── include
│   └── headers.h       # Header files
├── obj                 # Object files (generated during compilation)
├── bin                 # Binary executable (generated during linking)
├── Makefile
└── README.md
```




