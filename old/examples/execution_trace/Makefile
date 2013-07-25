IOSIG_LIB_PATH=$(HOME)/install/lib

main: main.o 
	gcc main.o -o main -L${IOSIG_LIB_PATH} -liosig


main.o: main.c
	gcc -finstrument-functions -g -c -o main.o main.c

clean:
	rm *.o main

