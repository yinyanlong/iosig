#!/bin/bash
rm -f ./exe*.out
rm -f ./mpiio*.out
rm -f ./posix*.out
rm -f ./exe*.out.new

# build the executable
make clean
make -f Makefile.iosig

# run the executable, this step also generates the 
# execution tracing file, named exe_pid.out
./main

# use the script to translate the trace to readable format
for trace in $(ls exe*.out)
do
	source ../../collect/readexelog.sh ./main $trace 2>&1 >> $trace.new
done
