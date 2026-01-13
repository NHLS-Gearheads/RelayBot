#include <Arduino.h>


// ===== Global variables ====== 
  int lastDirection = 0;
  int blocksSeen = 0;
  unsigned long startTime = 0;
  bool startTimeSet = false;
  bool leftTurn = false;

// ===== PIN CONFIGURATION =====
const int MOTOR_A_IN1 = 11;  // Direction control 1 (PWM)
const int MOTOR_A_IN2 = 10;  // Direction control 2 (PWM)

const int MOTOR_B_IN1 = 3;   // Direction control 1 (PWM)
const int MOTOR_B_IN2 = 9;   // Direction control 2 (PWM)

// Ultrasonic sensor pins
const int ULTRA_TRIG = 4;  // Trig pin
const int ULTRA_ECHO = 12;  // Echo pin

// Line sensor pins
const int SENS_LINE1 = A0;
const int SENS_LINE2 = A1;
const int SENS_LINE3 = A2;
const int SENS_LINE4 = A3;
const int SENS_LINE5 = A4;
const int SENS_LINE6 = A5;
const int SENS_LINE7 = A6;
const int SENS_LINE8 = A7;


// Gripper pin
const int GRIPPER = 13;

// ===== TIMING CONFIGURATION =====
// You'll need to calibrate this value for your specific robot
// Start with this and adjust based on testing
const int TURN_180_TIME = 950;  // Time in milliseconds to turn 180 degrees
const int METER_TIME = 2500;    // Time in milliseconds to drive 1 meter forwards or backwards
const int TURN_90_RIGHT = 3700; // Time in milliseconds to turn right 
const int TURN_90_LEFT = 350;  // Time in milliseconds to turn left

// ===== FUNCTION DECLARATIONS =====
void lineSensor();
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
  
  // Configure Motor A pins
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  
  // Configure Motor B pins
  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);
  
  // Configure Sonar pins
  pinMode(ULTRA_TRIG, OUTPUT);
  pinMode(ULTRA_ECHO, INPUT);

  // Configure Line sensor pins
  for (int i = 14; i < 22; i++) {
    pinMode(i, INPUT);
  }

  // Configure gripper pin
  pinMode(GRIPPER, OUTPUT);
  // Start with robot stopped
  robotStop();
  
  // robotForward(255);
  // digitalWrite(GRIPPER, HIGH);
  // delayMicroseconds(1025);
  // digitalWrite(GRIPPER, LOW);
  
  Serial.println("Setup complete. Starting test in 3 seconds...");
  delay(3000);
}

void loop() {
  lineSensor();
  readUltrasonicCM();
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

void robotTurnRightLight(int speed) {
  // Motor A - Forward 
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward with less speed
  analogWrite(MOTOR_B_IN1, speed-20);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnRightMedium(int speed) {
  // Motor A - Forward 
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward with less speed
  analogWrite(MOTOR_B_IN1, speed-40);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnRightHeavy(int speed) {
  // Motor A - Forward 
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward with less speed
  analogWrite(MOTOR_B_IN1, speed-100);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnLeftLight(int speed) {
  // Motor A - Forward with less speed
  analogWrite(MOTOR_A_IN1, speed-30);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward 
  analogWrite(MOTOR_B_IN1, speed);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnLeftMedium(int speed) {
  // Motor A - Forward with less speed
  analogWrite(MOTOR_A_IN1, speed-50);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward 
  analogWrite(MOTOR_B_IN1, speed);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnLeftHeavy(int speed) {
  // Motor A - Forward with less speed
  analogWrite(MOTOR_A_IN1, speed-110);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Forward 
  analogWrite(MOTOR_B_IN1, speed);
  analogWrite(MOTOR_B_IN2, 0);
}

void robotTurnRightFULL(int speed) {
  // Motor A - Forward
  analogWrite(MOTOR_A_IN1, speed);
  analogWrite(MOTOR_A_IN2, 0);
  
  // Motor B - Backward (same direction as Motor A)
  analogWrite(MOTOR_B_IN1, 0);
  analogWrite(MOTOR_B_IN2, speed);
}

void robotTurnLeftFULL(int speed) {
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

long readUltrasonicCM() {
  digitalWrite(ULTRA_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG, LOW);

  long duration = pulseIn(ULTRA_ECHO, HIGH);
  long distance = duration * 0.0343 / 2; // cm
  if(distance < 30) {
      	Serial.println("sensing object!!!!");
        robotTurnRightHeavy(255);
        delay(1000);
        robotTurnLeftHeavy(255);
        delay(2000);
        robotTurnRightHeavy(255);
        delay(1000);
        return distance;
  }
  return distance;
}

// 25-11-2025 14:38 the white paper shows as below 700 and the black table shows as above 950
void lineSensor(){
  int s1 = analogRead(SENS_LINE1);
  int s2 = analogRead(SENS_LINE2);
  int s3 = analogRead(SENS_LINE3);
  int s4 = analogRead(SENS_LINE4);
  int s5 = analogRead(SENS_LINE5);
  int s6 = analogRead(SENS_LINE6);
  int s7 = analogRead(SENS_LINE7);
  int s8 = analogRead(SENS_LINE8);
  Serial.println(String(s1) + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8);

  bool allBlack =
  s1 > 750 && s2 > 750 && s3 > 750 && s4 > 750 &&
  s5 > 750 && s6 > 750 && s7 > 750 && s8 > 750;

if (allBlack) {

  if (!startTimeSet) {
    startTime = millis();
    startTimeSet = true;
  }

  robotForward(255);

  if (millis() - startTime >= 250) {
    blocksSeen++;
    startTime += 250;
    Serial.println("block detected!");
  }

} else {
  // ONLY reset when leaving the black block
  startTimeSet = false;
}
  
  if(blocksSeen == 1) {
    digitalWrite(GRIPPER, HIGH);
    delayMicroseconds(1000);
    digitalWrite(GRIPPER, LOW);
    if (!leftTurn) {
    robotStop();
    delay(500);
    robotTurnLeftFULL(255);
    delay(TURN_90_LEFT);
    robotStop();
    leftTurn = true;
    }
  }
  else if(blocksSeen > 1 || blocksSeen == 0 && allBlack) {
    digitalWrite(GRIPPER, HIGH);
    delayMicroseconds(1600);
    digitalWrite(GRIPPER, LOW);
    blocksSeen = 0;
  }

  if(s1 > 750) {
      robotTurnRightHeavy(255);
      lastDirection = 1;
      readUltrasonicCM();
  } else if(s2 > 750) {
      robotTurnRightMedium(255);
      lastDirection = 1;
      readUltrasonicCM();
  } else if(s3 > 750) {
      robotTurnRightLight(255);
      lastDirection = 1;
      readUltrasonicCM();
  }  else if(s6 > 750) {
      robotTurnLeftLight(255);
      lastDirection = 2;
      readUltrasonicCM();
  } else if(s7 > 750) {
      robotTurnLeftMedium(255);
      lastDirection = 2;
      readUltrasonicCM();
  } else if(s8 > 750) {
      robotTurnLeftHeavy(255);
      lastDirection = 2;
      readUltrasonicCM();
  } else if(s4 > 750) {
      robotForward(255);
      lastDirection = 0;
      readUltrasonicCM();
  } else if(s5 > 750) {
      robotForward(255);
      lastDirection = 0;
      readUltrasonicCM();
  } else {
      if(lastDirection == 1) {
        robotTurnRightFULL(255);
      }
      else if(lastDirection == 2) {
        robotTurnLeftFULL(255);
      }
      
  }
}
