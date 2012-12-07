
main: main.o trace.o
	gcc main.o trace.o -o main

trace.o: trace.c
	gcc -c -o trace.o trace.c

main.o: main.c
	gcc -finstrument-functions -g -c -o main.o main.c


