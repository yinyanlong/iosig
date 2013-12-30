# generate PNG
set terminal png
set output "iorate.png"

set size 1,1
set multiplot

# plot read data
set origin 0,0.5
set size 1,0.5 
set style line 1 lt 1 lc rgb "red"
set title "READ rates over time"
set xlabel "Time (s)"
set ylabel "Bandwidth (MB\/s)"
set xrange [0: ]
set yrange [0: ]
print trace.'read.dat'
plot trace.'.read.dat' ls 1 title 'Read' with steps

# plot write data
set origin 0,0 
set size 1,0.5 
set style line 2 lt 1 lc rgb "blue"
set title "WRITE rates over time"
set xlabel "Time (s)"
set ylabel "Bandwidth (MB\/s)"
set xrange [0: ]
set yrange [0: ]
print trace.'write.dat'
plot trace.'.write.dat' ls 2 title 'Write' with steps

unset multiplot
set size 1,1



