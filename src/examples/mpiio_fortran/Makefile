#!/bin/bash

mpiioExample:	mpiioExample.o
	mpif77 mpiioExample.o -o mpiioExample
	
mpiioExample.o:
	mpif77 -c ./mpiioExample.f -o ./mpiioExample.o

clean:
	rm -f *.o mpiioExample

