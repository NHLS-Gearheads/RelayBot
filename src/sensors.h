
#ifndef SENSORS_H
#define SENSORS_H

// Ultrasonic Sensor reading
long readUltrasonicCM();

// Rotation Sensor reading
void turnDegrees(int degrees, int speed);
void moveTicks(int ticks, int speed);

#endif
