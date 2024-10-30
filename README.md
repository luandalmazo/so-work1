# Process Synchronization with Barrier and FIFO Queue

## Overview

This program demonstrates process synchronization using two mechanisms: a barrier and a FIFO (First-In-First-Out) queue. It creates multiple child processes that synchronize their execution at a barrier and manage their order of execution with a FIFO queue, using shared memory and semaphores.

## Authors

- Luan Matheus Trindade Dalmazo [lmtd21]
- Mateus de Oliveira Silva [mos20]

## Code Structure

This program creates a specified number of child processes to synchronize with a barrier and share access to a FIFO queue using shared memory. Here's a breakdown:

1. **Argument Handling**: Validates that exactly one argument (number of processes) is provided. Exits if it's missing or invalid.

2. **Shared Memory Setup**:
   - Creates shared memory for a barrier and a FIFO queue.
   - Attaches this memory to the parent process.

3. **Barrier and FIFO Initialization**:
   - Initializes a barrier with `num_processes + 1` to synchronize the child processes with the parent.
   - Initializes a FIFO queue structure.

4. **Child Process Creation**:
   - Creates `num_processes` child processes using a loop.
   - Each child sleeps for a random time, waits at the barrier with `process_barrier`, and then accesses the FIFO queue.

5. **Parent Synchronization and FIFO Access**:
   - The parent process waits at the barrier for all children, sleeps randomly, and then accesses the FIFO queue.

6. **Cleanup**:
   - The parent waits for all child processes to finish.
   - Destroys semaphores in the barrier and FIFO.
   - Detaches and deletes the shared memory.

7. **Exit**:
   - Returns `0` to signal successful completion.

This setup synchronizes processes using the barrier and coordinates shared access to the FIFO queue.

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




