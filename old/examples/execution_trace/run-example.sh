#!/bin/bash

# build the executable
make clean
make

# run the executable, this step also generates the 
# execution tracing file, named exe_pid.out
./main

# use the script to translate the trace to readable format
for trace in $(ls exe*.out)
do
	source ../../trace_collect/readexelog.sh ./main $trace 2>&1 | tee $trace.new
done
