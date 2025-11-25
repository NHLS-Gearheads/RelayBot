#include <Arduino.h>
#include <bot_config.h>
#include <movement.h>
#include <sensors.h>

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  
  // Configure Motor A pins
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  
  // Configure Motor B pins
  pinMode(MOTOR_B_IN3, OUTPUT);
  pinMode(MOTOR_B_IN4, OUTPUT);

  pinMode(ULTRA_TRIG, OUTPUT);
  pinMode(ULTRA_ECHO, INPUT);

  // Start with robot stopped
  robotStop();
  
  Serial.println("Setup complete. Starting test in 3 seconds...");
  delay(1000);
}

void loop() {
  
  // long dist = readUltrasonicCM();
  // if (dist > 0 && dist < 15) {        
  //     // --- TUNING VARIABLES ---
  //     int DODGE_TIME = TURN_90_TIME * 1.4; // Time to turn OUT
      
  //     robotStop();
  //     delay(200);
      
  //     // 1. Back up 
  //     robotBackward(180);
  //     delay(600);     
  //     robotStop();
  //     delay(100);

  //     // --- PHASE 1: DODGE OUT ---
  //     // Arc Left (Angling away)
  //     robotTurnLeftCurb(180); 
  //     delay(DODGE_TIME); 

  //     robotForward(180);
  //     delay(1000); 

  //     robotTurnRightCurb(180);
  //     delay(DODGE_TIME);

  //     robotTurnRightCurb(180);
  //     delay(DODGE_TIME);

  //     robotForward(180);
  //     delay(1500); 

  //     robotTurnLeftCurb(180);
  //     delay(850);

  //     robotStop();

  // } else {
  //   // Path is clear, keep moving
  //   robotForward(180);
  // }

  turnDegrees(90, 255);
  turnDegrees(-90, 255);

}