#ifndef SENSORS_H
#define SENSORS_H

// Ultrasonic Sensor reading
long readUltrasonicCM();

// Line Sensor reading
int readLineSensors();

// Rotation Sensor reading
void turnDegrees(int degrees, int speed);
void moveTicks(int targetTicks, int speed);

#endif
