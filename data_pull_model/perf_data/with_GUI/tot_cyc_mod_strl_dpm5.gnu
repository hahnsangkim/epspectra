set xlabel "udp transmitter"
set xtics ("StrlFileSource" 1, "StrlBits2Symbols" 2, "StrlDigMod" 3, "StrlSum" 4, "StrlUDPSink" 5, "StrlScopeSink" 6)
set ylabel "MCycles"
set y2label "%CPU"
set ytics nomirror
set y2tics
set title "Performance Results on 602.3 MHz machine (for 20.000000 seconds)"
set time
set grid
set boxwidth 0.33
set y2range [0:1]
plot [0:7][0:152] "-" title "cycles" w boxes
0.833333 0.292318
1.833333 0.570044
2.833333 24.781852
3.833333 25.103334
4.833333 46.343884
5.833333 138.424896
e
