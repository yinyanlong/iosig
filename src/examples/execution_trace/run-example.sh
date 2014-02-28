#!/bin/bash

# build the executable
make clean
make -f Makefile.iosig

# run the executable
./main

