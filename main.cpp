#include <Arduino.h>

// ===== PIN CONFIGURATION =====
const int MOTOR_A_IN1 = 10;  // Direction control 1 (PWM)
const int MOTOR_A_IN2 = 11;  // Direction control 2 (PWM)
const int MOTOR_A_EN = 8;    // Enable/Speed control

const int MOTOR_B_IN1 = 3;   // Direction control 1 (PWM)
const int MOTOR_B_IN2 = 9;   // Direction control 2 (PWM)
const int MOTOR_B_EN = 7;    // Enable/Speed control 

// Ultrasonic sensor pins
const int ULTRA_TRIG = 4;  // Trig pin
const int ULTRA_ECHO = 12;  // Echo pin

// ===== TIMING CONFIGURATION =====
// You'll need to calibrate this value for your specific robot
// Start with this and adjust based on testing
const int TURN_180_TIME = 950;  // Time in milliseconds to turn 180 degrees
const int METER_TIME = 2500;    // Time in milliseconds to drive 1 meter forwards or backwards
const int TURN_90_RIGHT = 3700; // Time in milliseconds to turn right 
const int TURN_90_LEFT = 3200;  // Time in milliseconds to turn left

// ===== FUNCTION DECLARATIONS =====
void ForwardBackward_1Meter();
void robotForward(int speed);
void robotBackward(int speed);
void robotStop();
void robotTurnRight180(int speed);
void robotTurnLeft180(int speed);
void robotTurnRight90(int speed);
void robotTurnLeft90(int speed);
long readUltrasonicCM(); 

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


  pinMode(ULTRA_TRIG, OUTPUT);
  pinMode(ULTRA_ECHO, INPUT);

  
  // Start with robot stopped
  robotStop();
  
  Serial.println("Setup complete. Starting test in 3 seconds...");
  delay(3000);
}

void loop() {
  robotTurnRight180(240); // 180 dg - speen
  delay(TURN_180_TIME);
  robotStop();
  delay(200);

  robotForward(240); // 1 meter forward
  delay(METER_TIME);
  robotBackward(240); // 1 meter backward
  delay(METER_TIME);
  robotStop();

  robotTurnLeft90(200); // Turn left
  delay(TURN_90_LEFT);
  robotStop();

  robotTurnRight90(200); // Turn right
  delay(TURN_90_RIGHT);
  robotStop();
  
  delay(1000);
}



// ===== ROBOT CONTROL FUNCTIONS =====

void robotForward(int speed) {
  // Motor A - Forward
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward (reversed direction)
  analogWrite(MOTOR_B_IN1, speed);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotBackward(int speed) {
  // Motor A - Backward
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, speed);
  
  // Motor B - Backward (reversed direction)
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, speed);
}

void robotTurnRight90(int speed) {
  // Motor A - Forward 
  analogWrite(MOTOR_A_IN1, speed+40);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward with less speed
  analogWrite(MOTOR_B_IN1, speed-20);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnLeft90(int speed) {
  // Motor A - Forward with less speed
  analogWrite(MOTOR_A_IN1, speed-40);
  analogWrite(MOTOR_A_IN2, 0);
  
  
  // Motor B - Forward 
  analogWrite(MOTOR_B_IN1, speed+30);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnRight180(int speed) {
  // Motor A - Forward
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Backward (same direction as Motor A)
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, speed);
}

void robotTurnLeft180(int speed) {
  // Motor A - Backward
  analogWrite(MOTOR_A_IN1, 0);
  analogWrite(MOTOR_A_IN2, speed);
  
  // Motor B - Forward (opposite of Motor A)
  analogWrite(MOTOR_B_IN1, speed);
  analogWrite(MOTOR_B_IN2, 0);
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
