#!/bin/bash

rm -f *out*

# build the executable
make clean
make -f Makefile.iosig

# run the executable
./simple_posix

