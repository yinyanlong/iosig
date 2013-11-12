#!/bin/bash

rm -f *out*

# build the executable
make clean
make -f Makefile.iosig

# run the executable, this step also generates the 
# execution tracing file, named exe_pid.out
./simple_posix

# use the script to translate the trace to readable format
for trace in $(ls exe*.out)
do
	source ../../collect/readexelog.sh ./simple_posix $trace 2>&1 >> $trace.new
done
