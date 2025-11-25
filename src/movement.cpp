#include <Arduino.h>
#include <bot_config.h>

void robotForward(int speed) {
  // Motor A - Forward
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward (reversed direction)
  analogWrite(MOTOR_B_IN3, speed);
  analogWrite(MOTOR_B_IN4, 0);
}

void robotBackward(int speed) {
  // Motor A - Backward
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, speed);
  
  // Motor B - Backward (reversed direction)
  analogWrite(MOTOR_B_IN3, 0);
  analogWrite(MOTOR_B_IN4, speed);
}

void robotTurnRight(int speed) {
  
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  
  analogWrite(MOTOR_B_IN3, 0);
  analogWrite(MOTOR_B_IN4, speed);
}
void robotTurnLeft(int speed) {
  
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, speed);
  
  
  analogWrite(MOTOR_B_IN3, speed);
  analogWrite(MOTOR_B_IN4, 0);
}

void robotTurnRight180(int speed) {
  // Motor A - Forward
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Backward (same direction as Motor A)
  analogWrite(MOTOR_B_IN3, 0);
  analogWrite(MOTOR_B_IN4, speed);
}

void robotTurnLeft180(int speed) {
  // Motor A - Backward
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, speed);
  
  // Motor B - Forward (opposite of Motor A)
  analogWrite(MOTOR_B_IN3, speed);
  analogWrite(MOTOR_B_IN4, 0);
}

void robotStop() {
  // Stop Motor A
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Stop Motor B
  analogWrite(MOTOR_B_IN3, 0);
  analogWrite(MOTOR_B_IN4, 0);
}

// Forward Backward 1 Meter

void ForwardBackward_1Meter() {
  // Go forward for METER_TIME
  Serial.println("  Moving forward 1 meter...");
  robotForward(240);
  delay(METER_TIME);

  // Stop for 1 second
  Serial.println("  Stopping for 1 second...");
  robotStop();
  delay(1000);

  // Go backward for METER_TIME
  Serial.println("  Moving backward 1 meter...");
  robotBackward(240);
  delay(METER_TIME);

  // Stop
  Serial.println("  Stopping.");
  robotStop();
  delay(1000);
}

void robotTurnLeftCurb(int speed) {
  // Calculate a slower speed for the inner wheel (e.g., 50%)
  int innerSpeed = speed * 0.6; 

  // Motor A (Left/Inner) -> FORWARD SLOW
  analogWrite(MOTOR_A_IN1, innerSpeed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B (Right/Outer) -> FORWARD FAST
  analogWrite(MOTOR_B_IN3, speed);
  analogWrite(MOTOR_B_IN4, 0);
}

void robotTurnRightCurb(int speed) {
  // Calculate a slower speed for the inner wheel
  int innerSpeed = speed * 0.6;

  // Motor A (Left/Outer) -> FORWARD FAST
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B (Right/Inner) -> FORWARD SLOW
  analogWrite(MOTOR_B_IN3, innerSpeed);
  analogWrite(MOTOR_B_IN4, 0);
}