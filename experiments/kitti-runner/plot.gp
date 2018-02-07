# 2D
plot "07.txt" using 4:12 title 'GROUND-TRUTH' with lines, "KeyFrameTrajectoryTUM.txt" using 2:4 title 'ORB-SLAM2' with lines
plot "07.txt" using 4:12 title 'GROUND-TRUTH' with lines, "KeyFrameTrajectoryKITTI.txt" using 4:12 title 'ORB-SLAM2' with lines

# 3D
splot "07.txt" using 4:12:8 title 'GROUND-TRUTH' with lines, "KeyFrameTrajectoryTUM.txt" using 2:4:3 title 'ORB-SLAM2' with lines
splot "07.txt" using 4:12:8 title 'GROUND-TRUTH' with lines, "KeyFrameTrajectoryKITTI.txt" using 4:12:8 title 'ORB-SLAM2' with lines
