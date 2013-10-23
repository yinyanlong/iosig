IOSIG_LIB_PATH = ../../collect
CFLAGS    += -finstrument-functions
LDFLAGS   += -L${IOSIG_LIB_PATH} -liosig

simple_posix: simple_posix.o 
	gcc simple_posix.o -o simple_posix ${LDFLAGS} 


simple_posix.o: simple_posix.c
	gcc ${CFLAGS} -g -c -o simple_posix.o simple_posix.c

clean:
	rm *.o simple_posix

