#!/bin/bash
make clean
make -f Makefile.iosig
mpiexec -np 2 ./example ./example.test.data


