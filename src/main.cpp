#include <Arduino.h>

// ================================================================
// ==================== SETTINGS & CALIBRATION ====================
// ================================================================

// 1. SPEED & MOTOR BALANCE
const int BASE_SPEED = 160;         // Base speed for the robot
const int RIGHT_MOTOR_OFFSET = 15;  // Compensation if robot pulls right (reduce if pulling left)

// 2. TURN DURATIONS (Your calibrated values)
int TURN_DURATION_90 = 450;   // Time for 90-degree turn
int TURN_DURATION_180 = 800;  // Time for 180-degree turn (U-Turn)

// 3. DISTANCE THRESHOLDS (cm)
const int GAP_THRESHOLD = 30;   // Distance greater than this implies an open path/corridor
const int WALL_STOP_DIST = 13;  // Stop if front wall is closer than this

// 4. MOVEMENT TIMINGS (ms)
const int JUNCTION_ALIGN_TIME = 250;  // Move forward slightly before turning to align wheels
const int NEW_PATH_ENTRY_TIME = 400;  // Move forward after turning to enter the new corridor

// ==================== PIN DEFINITIONS ===========================
const int MA1 = 10; const int MA2 = 11; // Left Motor Pins
const int MB1 = 3;  const int MB2 = 9;  // Right Motor Pins

const int TRIG_FRONT = 4; const int ECHO_FRONT = 12;
const int TRIG_RIGHT = 5; const int ECHO_RIGHT = 6;
const int TRIG_LEFT = 2;  const int ECHO_LEFT = 13; 

long distFront, distRight, distLeft;

// ==================== FUNCTION PROTOTYPES =======================
void driveAndAlign(int duration); // Wall following logic
void maneuver(int leftSpeed, int rightSpeed);
void turnLeft();
void turnRight();
void turnAround();
void stopMotors();
void reverseSlightly();
long readDistance(int trig, int echo);

void setup() {
  Serial.begin(9600);
  
  // Motor Pins Setup
  pinMode(MA1, OUTPUT); pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT); pinMode(MB2, OUTPUT);
  
  // Sensor Pins Setup
  pinMode(TRIG_FRONT, OUTPUT); pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);  pinMode(ECHO_LEFT, INPUT);

  // Disable Encoder Pins (to avoid conflict)
  pinMode(7, INPUT); pinMode(8, INPUT); 

  delay(3000); // 3-second startup delay
}

void loop() {
  // 1. STOP AND SCAN
  stopMotors();
  delay(300); // Wait for sensors to stabilize
  
  distLeft  = readDistance(TRIG_LEFT, ECHO_LEFT);
  distRight = readDistance(TRIG_RIGHT, ECHO_RIGHT);
  distFront = readDistance(TRIG_FRONT, ECHO_FRONT);
  
  // Debugging
  Serial.print("L:"); Serial.print(distLeft);
  Serial.print(" F:"); Serial.print(distFront);
  Serial.print(" R:"); Serial.println(distRight);

  // === SAFETY CHECK: ANTI-STUCK ===
  if (distFront > 0 && distFront < 5) {
    reverseSlightly();
    return;
  }

  // === LOGIC: LEFT HAND RULE ===
  // Priority: LEFT -> FRONT -> RIGHT -> BACK

  // 1. PRIORITY: IS LEFT OPEN? -> TURN LEFT
  if (distLeft > GAP_THRESHOLD) {
    Serial.println("DECISION: LEFT OPEN -> TURNING LEFT");
    
    // Align with junction
    driveAndAlign(JUNCTION_ALIGN_TIME);
    stopMotors();
    delay(100);
    
    turnLeft();
    
    // Enter new path
    driveAndAlign(NEW_PATH_ENTRY_TIME);
  }
  
  // 2. PRIORITY: IS FRONT OPEN? -> GO STRAIGHT
  // (Only check if Left is blocked)
  else if (distFront > WALL_STOP_DIST) {
    Serial.println("DECISION: FRONT OPEN -> GOING STRAIGHT");
    // Drive straight while correcting position (Wall Following)
    driveAndAlign(400); 
  }
  
  // 3. PRIORITY: IS RIGHT OPEN? -> TURN RIGHT
  // (Only check if Left and Front are blocked)
  else if (distRight > GAP_THRESHOLD) {
    Serial.println("DECISION: RIGHT OPEN -> TURNING RIGHT");
    
    driveAndAlign(JUNCTION_ALIGN_TIME);
    stopMotors();
    delay(100);
    
    turnRight();
    
    driveAndAlign(NEW_PATH_ENTRY_TIME);
  }
  
  // 4. DEAD END -> TURN AROUND
  else {
    Serial.println("DECISION: DEAD END -> U-TURN");
    turnAround();
  }
}

// ==================== MOVEMENT FUNCTIONS ====================

// This function keeps the robot centered between walls while moving forward
void driveAndAlign(int duration) {
  unsigned long startTime = millis();
  
  while(millis() - startTime < (unsigned long)duration) {
    long currentLeft = readDistance(TRIG_LEFT, ECHO_LEFT);
    long currentRight = readDistance(TRIG_RIGHT, ECHO_RIGHT);
    
    // Safety: Stop immediately if a wall appears in front
    if (readDistance(TRIG_FRONT, ECHO_FRONT) < WALL_STOP_DIST) {
      stopMotors();
      break;
    }

    // --- WALL FOLLOWING LOGIC ---
    // If too close to LEFT wall (< 8cm) -> Steer RIGHT
    if (currentLeft < 8 && currentLeft > 0) {
      maneuver(BASE_SPEED + 20, BASE_SPEED - 10); 
    }
    // If too close to RIGHT wall (< 8cm) -> Steer LEFT
    else if (currentRight < 8 && currentRight > 0) {
      maneuver(BASE_SPEED - 20, BASE_SPEED + 30); 
    }
    // Otherwise -> Drive Straight with Offset
    else {
      maneuver(BASE_SPEED, BASE_SPEED + RIGHT_MOTOR_OFFSET);
    }
    
    delay(20); // Small delay for loop stability
  }
  stopMotors();
}

void maneuver(int leftSpeed, int rightSpeed) {
  // Constrain speeds to 0-255
  if (leftSpeed > 255) leftSpeed = 255; if (leftSpeed < 0) leftSpeed = 0;
  if (rightSpeed > 255) rightSpeed = 255; if (rightSpeed < 0) rightSpeed = 0;
  
  analogWrite(MA1, leftSpeed); analogWrite(MA2, 0);
  analogWrite(MB1, rightSpeed); analogWrite(MB2, 0);
}

void turnLeft() {
  // Tank Turn Left
  analogWrite(MA1, 0);          analogWrite(MA2, BASE_SPEED);
  analogWrite(MB1, BASE_SPEED); analogWrite(MB2, 0);
  delay(TURN_DURATION_90);
  stopMotors();
  delay(200);
}

void turnRight() {
  // Tank Turn Right
  analogWrite(MA1, BASE_SPEED); analogWrite(MA2, 0);
  analogWrite(MB1, 0);          analogWrite(MB2, BASE_SPEED);
  delay(TURN_DURATION_90);
  stopMotors();
  delay(200);
}

void turnAround() {
  // U-Turn (Right)
  analogWrite(MA1, BASE_SPEED); analogWrite(MA2, 0);
  analogWrite(MB1, 0);          analogWrite(MB2, BASE_SPEED);
  delay(TURN_DURATION_180);
  stopMotors();
  delay(200);
}

void reverseSlightly() {
  // Back up slightly if stuck
  analogWrite(MA1, 0); analogWrite(MA2, BASE_SPEED);
  analogWrite(MB1, 0); analogWrite(MB2, BASE_SPEED + RIGHT_MOTOR_OFFSET);
  delay(350);
  stopMotors();
}

void stopMotors() {
  digitalWrite(MA1, LOW); digitalWrite(MA2, LOW);
  digitalWrite(MB1, LOW); digitalWrite(MB2, LOW);
}

long readDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 6000); // 6ms timeout (approx 1m range)
  
  if (duration == 0) return 999; // Return 999 if no echo (open space)
  return duration * 0.0343 / 2;
}