#!/bin/bash
rm -f ./trace*.out
rm -f ./exe*.out
rm -f ./mpiio*.out
rm -f ./posix*.out
rm -f ./exe*.out.new

#make clean
#make

gdb --args mpiexec -np 1 ./IOR -a MPIIO -b 24m -o /tmp/datafile -w -k -t 4k

