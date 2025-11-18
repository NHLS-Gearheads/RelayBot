#include <Arduino.h>

// ===== PIN CONFIGURATION =====
const int MOTOR_A_IN1 = 11;  // Direction control 1 (PWM)
const int MOTOR_A_IN2 = 10;  // Direction control 2 (PWM)
const int MOTOR_A_EN = 8;    // Enable/Speed control

const int MOTOR_B_IN1 = 9;   // Direction control 1 (PWM)
const int MOTOR_B_IN2 = 3;   // Direction control 2 (PWM)
const int MOTOR_B_EN = 7;    // Enable/Speed control 

// ===== TIMING CONFIGURATION =====
// You'll need to calibrate this value for your specific robot
// Start with this and adjust based on testing
const int TURN_180_TIME = 925;  // Time in milliseconds to turn 180 degrees
const int METER_TIME = 4000;    // Time in milliseconds to drive 1 meter forwards or backwards

// ===== FUNCTION DECLARATIONS =====
void ForwardBackward_1Meter();
void robotForward(int speed);
void robotStop();
void robotTurnRight(int speed);
void robotTurnLeft(int speed);

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  Serial.println("Robot 180 Degree Spin Test");
  Serial.println("==========================");
  
  // Configure Motor A pins
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_A_EN, OUTPUT);
  
  // Configure Motor B pins
  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);
  pinMode(MOTOR_B_EN, OUTPUT);
  
  // Enable both motors
  digitalWrite(MOTOR_A_EN, HIGH);
  digitalWrite(MOTOR_B_EN, HIGH);
  
  // Start with robot stopped
  robotStop();
  
  Serial.println("Setup complete. Starting test in 3 seconds...");
  delay(3000);
}

void loop() {
  Serial.println("\n=== Starting 180 degree rotation sequence ===\n");
  
  // Turn right 180 degrees
  Serial.println("Turning RIGHT 180 degrees...");
  robotTurnRight(200);  // Turn at speed 200
  delay(TURN_180_TIME);
  robotStop();
  
  Serial.println("Stopped. Waiting 2 seconds...");
  delay(2000);
  
  // Turn left 180 degrees (back to original position)
  Serial.println("Turning LEFT 180 degrees (returning to start)...");
  robotTurnLeft(200);  // Same speed for consistency
  delay(TURN_180_TIME);
  robotStop();
  
  Serial.println("Back to original position!");
  Serial.println("=== Waiting 5 seconds before Forward Backward 1 Meter ===");
  delay(5000);

  // The complete Forward Backward function
  Serial.print("Starting Forward Backward function...");
  ForwardBackward_1Meter();
  Serial.println("\n=== Waiting 5 seconds before restart ===\n");
}

// ===== ROBOT CONTROL FUNCTIONS =====

void robotForward(int speed) {
  // Motor A - Forward
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward (reversed direction)
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, speed);
}

void robotTurnRight(int speed) {
  // Motor A - Forward
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Backward (same direction as Motor A)
  analogWrite(MOTOR_B_IN1, speed);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnLeft(int speed) {
  // Motor A - Backward
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, speed);
  
  // Motor B - Forward (opposite of Motor A)
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, speed);
}

void robotStop() {
  // Stop Motor A
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Stop Motor B
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, 0);
}

// Forward Backward 1 Meter

void ForwardBackward_1Meter() {
  analogWrite(MOTOR_A_IN1, 230);
  analogWrite(MOTOR_A_IN2, 0);
  analogWrite(MOTOR_B_IN1, 255);
  analogWrite(MOTOR_B_IN2, 0);
  millis(METER_TIME);
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, 0);
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, 0);
  millis(1000);
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, 255);
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, 250);
  millis(METER_TIME);
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, 0);
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, 0);
  millis(5000);
}