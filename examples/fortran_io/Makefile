#mpif77 -c ./mpiioExample.f -o ./mpiioExample.o
#mpif77 mpiioExample.o -o a.out -L/export/home/yyin/install/lib/ -liosig
#mpif77 mpiioExample.o -o a.out -L/export/home/yyin/research/iosig/trace_collect_fortran/tcf/ -liosig
#mpiexec -np 4 ./a.out

LD_FLAGS	:= -L/export/home/yyin/research/iosig/trace_collect_fortran/tcf/ -liosig

mpiioExample:	mpiioExample.o
	mpif77 mpiioExample.o -o mpiioExample -$LD_FLAGS
	
mpiioExample.o:
	mpif77 -c ./mpiioExample.f -o ./mpiioExample.o

clean:
	rm -f *.o
	rm -f mpiioExample
