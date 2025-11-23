#ifndef MOVEMENT_H
#define MOVEMENT_H

// Basic Movement
void robotForward(int speed);
void robotBackward(int speed);
void robotStop();

// Turning
void robotTurnRight90(int speed);
void robotTurnLeft90(int speed);
void robotTurnRight180(int speed);
void robotTurnLeft180(int speed);

// Curb
void robotTurnLeftCurb(int speed);
void robotTurnRightCurb(int speed);

// Complex Sequences
void ForwardBackward_1Meter();

#endif