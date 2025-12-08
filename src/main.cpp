#include <Arduino.h>
#include "bot_config.h"
#include "movement.h"
#include "sensors.h"

// ===== Global variables ====== 
int lastDirection = 0;

// ===== LINE FOLLOWER CONSTANTS =====
const int BASE_SPEED = 180;  
const int MAX_SPEED = 255;
const float Kp = 0.15;       

// ===== FUNCTION DECLARATIONS =====
void lineSensor();

void setup() {
  Serial.begin(9600);
  Serial.println("Robot Line Follower");
  Serial.println("====================");

  // Configure Motor A pins
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  
  // Configure Motor B pins
  pinMode(MOTOR_B_IN3, OUTPUT);
  pinMode(MOTOR_B_IN4, OUTPUT);

  // Configure Sonar pins
  pinMode(ULTRA_TRIG, OUTPUT);
  pinMode(ULTRA_ECHO, INPUT);

  // Configure Line sensor IR enable pin
  pinMode(IR_ENABLE_PIN, OUTPUT);
  digitalWrite(IR_ENABLE_PIN, HIGH); // Enable the IR sensor array

  // Start with robot stopped
  robotStop();
  
  Serial.println("Setup complete. Starting test in 3 seconds...");
  delay(3000);
}

void loop() {
  lineSensor();
}

// ===== LINE FOLLOWER LOGIC =====
void lineSensor() {
  int s1 = analogRead(IR_PIN_1); // D1 - Far Right
  int s2 = analogRead(IR_PIN_2); // D2
  int s3 = analogRead(IR_PIN_3); // D3
  int s4 = analogRead(IR_PIN_4); // D4 - Center
  int s5 = analogRead(IR_PIN_5); // D5
  int s6 = analogRead(IR_PIN_6); // D6
  int s7 = analogRead(IR_PIN_7); // D7 - Far Left

  Serial.println(String(s1) + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7);

  // Lost line detection
  if (s1 < THRESHOLD && s2 < THRESHOLD && s3 < THRESHOLD && s4 < THRESHOLD && 
      s5 < THRESHOLD && s6 < THRESHOLD && s7 < THRESHOLD) {
      if (lastDirection == 1) robotTurnRight(130); 
      else if (lastDirection == 2) robotTurnLeft(130);  
      return;
  }

  // Calculate error using weighted positions (7 sensors)
  // Weights: s7(+3), s6(+2), s5(+1) for left | s4(0) center | s3(-1), s2(-2), s1(-3) for right
  long error = (s7 * 3 + s6 * 2 + s5 * 1) - (s3 * 1 + s2 * 2 + s1 * 3);
  int turnAdjustment = error * Kp;

  int leftSpeed = constrain(BASE_SPEED - turnAdjustment, 0, MAX_SPEED);
  int rightSpeed = constrain(BASE_SPEED + turnAdjustment, 0, MAX_SPEED);

  analogWrite(MOTOR_A_IN1, rightSpeed);
  analogWrite(MOTOR_A_IN2, 0);
  analogWrite(MOTOR_B_IN3, leftSpeed);
  analogWrite(MOTOR_B_IN4, 0);

  if (turnAdjustment < -50) lastDirection = 1;
  else if (turnAdjustment > 50) lastDirection = 2;
}