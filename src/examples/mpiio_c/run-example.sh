#!/bin/bash
rm -f ./exe*.out
rm -f ./mpiio*.out
rm -f ./posix*.out
rm -f ./exe*.out.new

make clean
make -f Makefile.iosig
mpiexec -np 2 ./example ./example.test.data 2>&1 | tee strace.log

# use the script to translate the trace to readable format
for trace in $(ls exe*.out)
do
	source ../../collect/readexelog.sh ./example $trace 2>&1 >> $trace.new
done

