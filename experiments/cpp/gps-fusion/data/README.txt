1. *.txt contains poses representing camera trajectory
	column 1: 			timestamp(dummy)
	column 2, 3, 4: 	xyz
	column 5, 6, 7, 8: 	quaternion(q0-q3)
	column 9, 10: 		dummy gps position in local coordinate system
	
	
2. *.g2o 
	contains the graph created by orb-slam2. lba=local bundle adjustment
	posopt = pose optimization.
	
	g2o files can be loaded using optimizer.load(), extra constraints can be added on top of this.
	Its one way to simulate bundle adjustment problem, if we can delete some landmark vertices if required.
	
	advantage of using g2o file from orb-slam2 is we can work on real world covariance values