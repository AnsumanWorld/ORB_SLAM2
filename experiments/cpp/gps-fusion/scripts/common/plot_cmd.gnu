# GPS Fusion
plot "experiments/gps-fusion/data/every-frame-gps.txt" using 2:3 title 'VO' with points, "experiments/gps-fusion/data/every-frame-gps.txt" using 9:10 title 'GPS' with points, "KeyFrameTrajectoryOptimized.txt" using 1:2 title 'OptGPS' with lines

# GPS BA
plot 'cam-before.txt' using 1:2 title 'vo-before' with lines, 'cam-after.txt' using 1:2 title 'vo+gps' with lines, 'gps.txt' using 1:2 title 'gps' with points
