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
set y2range [0:3]
plot [0:7][0:327] "-" title "cycles" w boxes
0.833333 0.677830
1.833333 1.281482
2.833333 59.482228
3.833333 53.438392
4.833333 121.276032
5.833333 297.478048
e
