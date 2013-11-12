#!/bin/bash
rm -f ./trace*.out
rm -f ./exe*.out
rm -f ./mpiio*.out
rm -f ./posix*.out
rm -f ./exe*.out.new

make clean
make

mpiexec -np 1 ./IOR -a MPIIO -b 24m -o /tmp/datafile -w -k -t 4k

# use the script to translate the trace to readable format
for trace in $(ls exe*.out)
do
	source ../../collect/readexelog.sh ./IOR $trace 2>&1 >> $trace.new
done
