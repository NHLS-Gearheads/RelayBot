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

// ===== FUNCTION DECLARATIONS =====
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
  Serial.println("\n=== Waiting 5 seconds before restart ===\n");
  delay(5000);
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