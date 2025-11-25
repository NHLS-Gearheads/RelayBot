#include <Arduino.h>
#include <movement.h>
#include <bot_config.h>

long readUltrasonicCM() {
  digitalWrite(ULTRA_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG, LOW);

  long duration = pulseIn(ULTRA_ECHO, HIGH);
  long distance = duration * 0.0343 / 2; // cm
  return distance;
}

bool prevLeftState = false;
bool prevRightState = false;

// The Function to Turn Degrees
void turnDegrees(int degrees, int speed) {
  // 1. Calculate Target Ticks
  float targetTicksFloat = abs(degrees) * TICKS_PER_DEGREE;
  int targetTicks = round(targetTicksFloat);
  
  // Safety check for low resolution encoders
  if (targetTicks < 1) targetTicks = 1;

  Serial.print("Turning ");
  Serial.print(degrees);
  Serial.print(" deg. Target Ticks: ");
  Serial.println(targetTicks);

  // 2. Reset Counters
  int currentTicks = 0;
  
  // Read initial states
  prevLeftState = (analogRead(SENSOR_PIN_LEFT) > THRESHOLD);
  prevRightState = (analogRead(SENSOR_PIN_RIGHT) > THRESHOLD);

  // 3. Start Motors based on direction
  if (degrees > 0) {
    // --- RIGHT TURN (+90) ---
    // Robot spins Clockwise
    robotTurnRight(speed);
    
  } else {
    // --- LEFT TURN (-90) ---
    // Robot spins Counter-Clockwise
    robotTurnLeft(speed);

  }

  // 4. Blocking Loop: Count Ticks until Target Reached
  while (currentTicks < targetTicks) {
    
    // --- Read Left Sensor ---
    bool currLeftState = (analogRead(SENSOR_PIN_LEFT) > THRESHOLD);
    if (currLeftState != prevLeftState) {
      if (currLeftState == true) currentTicks++; 
      prevLeftState = currLeftState;
    }

    // --- Read Right Sensor ---
    bool currRightState = (analogRead(SENSOR_PIN_RIGHT) > THRESHOLD);
    if (currRightState != prevRightState) {
      if (currRightState == true) currentTicks++; 
      prevRightState = currRightState;
    }
  }

  // 5. Target Reached -> Stop
  robotStop();
  Serial.println("Turn Complete.");
  delay(500); // Settle time
}