#!/bin/bash

simple_posix: simple_posix.o 
	gcc simple_posix.o -o simple_posix

simple_posix.o: simple_posix.c
	gcc -c -o simple_posix.o simple_posix.c

clean::
	rm *.o simple_posix 

