IOSIG_LIB_PATH = ../../collect
CFLAGS    += -finstrument-functions
LDFLAGS   += -L${IOSIG_LIB_PATH} -liosig

mpiioExample:	mpiioExample.o
	mpif77 mpiioExample.o -o mpiioExample $(LDFLAGS)
	
mpiioExample.o:
	mpif77 -c ${CFLAGS} ./mpiioExample.f -o ./mpiioExample.o

clean:
	rm -f *.o trace*.out mpiioExample exe*.out

