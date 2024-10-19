CC     = gcc
CFLAGS = -std=c11 -Wall
LFLAGS =

PROG = main
OBJ = barrier.o $(PROG).o

.PHONY: clean purge all

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	@rm -f *~ *.bak *.o main

purge: clean	
	@rm -rf bin obj *.layout *.depend
	@rm -f *.o core a.out
	@rm -f $(PROG)
