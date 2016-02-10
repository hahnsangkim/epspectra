set xlabel "udp transmitter"
set xtics ("Running Time" 1, "Computing Time" 2, "Scheduling Time" 3)
set ylabel "MCycles"
set y2label "%CPU"
set ytics nomirror
set y2tics
set title "Performance Results on 602.3 MHz machine (for 20.000000 seconds)"
set time
set grid
set boxwidth 0.33
set y2range [0:109]
plot [0:4][0:13155] "-" title "cycles" w boxes
0.833333 11959.148544
1.833333 6140.223488
2.833333 5818.925056
e
