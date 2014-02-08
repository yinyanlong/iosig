# generate PNG
set size 1,1
set terminal png size 1200,240
set output "iorate.png"

#set size ratio 0.2
set multiplot

# plot read data
set origin 0,0.5
set size 1,0.5 
set style line 1 lt 1 lc rgb "red"
set datafile separator ','
set key autotitle columnhead
#set title "READ rates over time"
#set xlabel "Time (s)"
#set ylabel "Bandwidth (MB\/s)"
set xrange [0: ]
set yrange [0: ]
print trace.'.read.rate.csv'
plot trace.'.read.rate.csv' ls 1 title 'Read' with steps

# plot write data
set origin 0,0 
set size 1,0.5 
set datafile separator ','
set key autotitle columnhead
set style line 2 lt 1 lc rgb "blue"
#set title "WRITE rates over time"
#set xlabel "Time (s)"
#set ylabel "Bandwidth (MB\/s)"
set xrange [0: ]
set yrange [0: ]
print trace.'write.dat'
plot trace.'.write.rate.csv' ls 2 title 'Write' with steps

unset multiplot
set size 1,1



