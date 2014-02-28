#!/bin/bash

CC           = mpicc

EXECS        = example

all: $(EXECS)

example: example.o
	$(CC) -o $@ $^

clean:
	rm -f *.o ./example

.c.o:
	$(CC) -c $<

