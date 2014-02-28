#!/bin/bash

make clean
make -f Makefile.iosig

mpiexec -np 1 ./IOR -a MPIIO -b 24m -o /tmp/datafile -w -k -t 4k

