#include "LightSensors.h"
#include "MotorControl.h"

// Read all sensor values into the global array
void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);  // Read raw analog value (0-1023)
  }
}

// Calibrate sensors by spinning in place to sweep over line and background
void calibrateSensors() {
  static bool firstRun = true;
  static unsigned long calibrationStart = 0;
  const unsigned long CALIBRATION_DURATION = 2000;  // Spin for 2 seconds
  
  readSensors();

  // On first run, initialize and start spinning
  if (firstRun) {
    for (int i = 0; i < NUM_SENSORS; i++) {
      sensorMin[i] = 1023;
      sensorMax[i] = 0;
    }
    calibrationStart = millis();
    Serial.println("R2 Calibrating... spin in place");
    
    // Start spinning in place (left motor forward, right motor backward)
    analogWrite(PIN_LEFT_FWD, 150);
    digitalWrite(PIN_LEFT_BWD, LOW);
    digitalWrite(PIN_RIGHT_FWD, LOW);
    analogWrite(PIN_RIGHT_BWD, 150);
    
    firstRun = false;
  }

  // Check if calibration time elapsed
  if (millis() - calibrationStart >= CALIBRATION_DURATION) {
    stopMotors();
    sensorsCalibrated = true;

    // Calculate threshold as midpoint between min and max
    for (int i = 0; i < NUM_SENSORS; i++) {
      sensorThreshold[i] = (sensorMin[i] + sensorMax[i]) / 2;
      Serial.print("Threshold [");
      Serial.print(i);
      Serial.print("]: ");
      Serial.print(sensorThreshold[i]);
      Serial.print(" (min:");
      Serial.print(sensorMin[i]);
      Serial.print(" max:");
      Serial.print(sensorMax[i]);
      Serial.println(")");
    }
    Serial.println("R2 Calibration complete!");
    return;
  }

  // Update min/max values for each sensor
  for (int i = 0; i < NUM_SENSORS; i++) {
    int sensorValue = analogRead(sensorPins[i]);
    if (sensorValue < sensorMin[i]) {
      sensorMin[i] = sensorValue;
    }
    if (sensorValue > sensorMax[i]) {
      sensorMax[i] = sensorValue;
    }
  }
}

// Calculate weighted position of line under sensors
int calculateLinePosition() {
  long weightedSum = 0;  // Sum of (sensor reading × position)
  long sum = 0;          // Sum of all sensor readings

  // Calculate weighted average of sensor readings
  for (int i = 0; i < NUM_SENSORS; i++) {
    int value = sensorValues[i];
    weightedSum += (long)value * i * 1000;  // Multiply by position (×1000 for precision)
    sum += value;                           // Add to total
  }

  return weightedSum / sum;  // Return weighted position (higher = more to the right)
} 