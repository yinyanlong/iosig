#!/bin/bash

#python ../trace_analyzer/sig.py -m ../../standard.properties -f ./log4 -d 
#cd ./result_output
#gnuplot ../ior.rw.png.plt
#ps2pdf ior4.ps
#cd ..


for tracefile in posix_trace*out
do
    echo $tracefile
    python $IOSIG_HOME/src/analysis/sig.py -m $IOSIG_HOME/src/analysis/format.properties -f $tracefile

    cd ./result_output
    gnuplot -e "trace='${tracefile}'" $IOSIG_HOME/src/analysis/ior.rw.png.plt.2
    mv iorate.png ${tracefile}.iorate.png
    cd ..
done
