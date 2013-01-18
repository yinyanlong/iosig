
trace.o: trace.c
	gcc -c -o trace.o trace.c

all: main

main: main.o trace.o
	gcc main.o trace.o -o main


main.o: main.c
	gcc -finstrument-functions -g -c -o main.o main.c

clean:
	rm *.o

