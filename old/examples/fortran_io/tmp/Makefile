#LD_FLAGS	:= -L/export/home/yyin/research/iosig/trace_collect_fortran/tcf/ -liosig
LD_FLAGS	:= -L/export/home/yyin/install/lib/ -liosig

mpiioExample:	mpiioExample.o
	mpif77 mpiioExample.o -o mpiioExample $(LD_FLAGS)
	
mpiioExample.o:
	mpif77 -c ./mpiioExample.f -o ./mpiioExample.o

clean:
	rm -f *.o mpiioExample

