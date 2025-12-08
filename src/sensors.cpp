#include <Arduino.h>
#include "bot_config.h"
#include "movement.h"
#include "sensors.h"

long readUltrasonicCM() {
  digitalWrite(ULTRA_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG, LOW);

  long duration = pulseIn(ULTRA_ECHO, HIGH);
  long distance = duration * 0.0343 / 2;
  return distance;
}

// Returns a bitmask of which sensors see the line (7 sensors)
// Uses analogRead because A6 and A7 are analog-only pins
int readLineSensors() {
  int sensorValue = 0;
  if (analogRead(IR_PIN_7) > THRESHOLD) sensorValue |= (1 << 6); // Leftmost
  if (analogRead(IR_PIN_6) > THRESHOLD) sensorValue |= (1 << 5);
  if (analogRead(IR_PIN_5) > THRESHOLD) sensorValue |= (1 << 4);
  if (analogRead(IR_PIN_4) > THRESHOLD) sensorValue |= (1 << 3); // Center
  if (analogRead(IR_PIN_3) > THRESHOLD) sensorValue |= (1 << 2);
  if (analogRead(IR_PIN_2) > THRESHOLD) sensorValue |= (1 << 1);
  if (analogRead(IR_PIN_1) > THRESHOLD) sensorValue |= (1 << 0); // Rightmost
  return sensorValue;
}

void moveTicks(int targetTicks, int speed) {
  int leftTicks = 0;
  int rightTicks = 0;
  bool prevLeftState, prevRightState;
  bool currLeftState, currRightState;

  prevLeftState = (analogRead(SENSOR_PIN_LEFT) > ENCODER_THRESHOLD);
  prevRightState = (analogRead(SENSOR_PIN_RIGHT) > ENCODER_THRESHOLD);

  robotForward(speed);

  while (leftTicks < targetTicks || rightTicks < targetTicks) {
    currLeftState = (analogRead(SENSOR_PIN_LEFT) > ENCODER_THRESHOLD);
    currRightState = (analogRead(SENSOR_PIN_RIGHT) > ENCODER_THRESHOLD);

    if (currLeftState != prevLeftState) {
      leftTicks++;
      prevLeftState = currLeftState;
    }
    if (currRightState != prevRightState) {
      rightTicks++;
      prevRightState = currRightState;
    }
  }

  robotStop();
}

void turnDegrees(int degrees, int speed) {
  int targetTicks = abs(degrees) * TICKS_PER_DEGREE;
  int leftTicks = 0;
  int rightTicks = 0;
  bool prevLeftState, prevRightState;
  bool currLeftState, currRightState;

  prevLeftState = (analogRead(SENSOR_PIN_LEFT) > ENCODER_THRESHOLD);
  prevRightState = (analogRead(SENSOR_PIN_RIGHT) > ENCODER_THRESHOLD);

  if (degrees > 0) {
    robotTurnRight(speed);
  } else {
    robotTurnLeft(speed);
  }

  while (leftTicks < targetTicks || rightTicks < targetTicks) {
    currLeftState = (analogRead(SENSOR_PIN_LEFT) > ENCODER_THRESHOLD);
    currRightState = (analogRead(SENSOR_PIN_RIGHT) > ENCODER_THRESHOLD);

    if (currLeftState != prevLeftState) {
      leftTicks++;
      prevLeftState = currLeftState;
    }
    if (currRightState != prevRightState) {
      rightTicks++;
      prevRightState = currRightState;
    }
  }

  robotStop();
}