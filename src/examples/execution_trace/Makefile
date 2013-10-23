IOSIG_LIB_PATH = ../../collect
CFLAGS    += -finstrument-functions
LDFLAGS   += -L${IOSIG_LIB_PATH} -liosig

main: main.o 
	gcc main.o -o main ${LDFLAGS}


main.o: main.c
	gcc ${CFLAGS} -g -c -o main.o main.c

clean:
	rm *.o main

