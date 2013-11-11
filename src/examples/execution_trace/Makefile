#!/bin/bash

main: main.o 
	gcc main.o -o main


main.o: main.c
	gcc -c -o main.o main.c

clean:
	rm *.o main

