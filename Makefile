# Set compiler and flags
CC     = gcc
CFLAGS = -std=c11 -Wall
LFLAGS =

# Define project directories and target
PROG = main

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Get source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Build rules
all: $(PROG)

$(PROG): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Cleaning rules
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

purge: clean
	rm -rf bin obj *.layout *.depend *.o core a.out $(PROG)

.PHONY: clean purge all