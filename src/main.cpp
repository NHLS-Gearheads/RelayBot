#include <Arduino.h>

// ================================================================
// ==================== SETTINGS & CALIBRATION ====================
// ================================================================

// 1. SPEED & MOTOR BALANCE
const int BASE_SPEED = 160;        // Base speed for the robot
const int RIGHT_MOTOR_OFFSET = 15; // Compensation if robot pulls right (reduce if pulling left)

// 2. TURN DURATIONS (Your calibrated values)
int TURN_DURATION_90 = 450;  // Time for 90-degree turn
int TURN_DURATION_180 = 800; // Time for 180-degree turn (U-Turn)

// 3. DISTANCE THRESHOLDS (cm)
const int GAP_THRESHOLD = 30;  // Distance greater than this implies an open path/corridor
const int WALL_STOP_DIST = 13; // Stop if front wall is closer than this

// 4. MOVEMENT TIMINGS (ms)
const int JUNCTION_ALIGN_TIME = 250; // Move forward slightly before turning to align wheels
const int NEW_PATH_ENTRY_TIME = 400; // Move forward after turning to enter the new corridor

// ==================== PIN DEFINITIONS ===========================
const int MA1 = 10;
const int MA2 = 11; // Left Motor Pins
const int MB1 = 3;
const int MB2 = 9; // Right Motor Pins

const int TRIG_FRONT = 4;
const int ECHO_FRONT = 12;
const int TRIG_RIGHT = 5;
const int ECHO_RIGHT = 6;
const int TRIG_LEFT = 13;
const int ECHO_LEFT = 2;

long distFront, distRight, distLeft;

const int GP = 7;

bool cal = false;
int changed = 1;


// Line sensor pins
const int SENS_LINE1 = A0;
const int SENS_LINE2 = A1;
const int SENS_LINE3 = A2;
const int SENS_LINE4 = A3;
const int SENS_LINE5 = A4;
const int SENS_LINE6 = A5;
const int SENS_LINE7 = A6;
const int SENS_LINE8 = A7;

const int SENS_ARRAY[8] = {
  SENS_LINE1,
  SENS_LINE2,
  SENS_LINE3,
  SENS_LINE4,
  SENS_LINE5,
  SENS_LINE6,
  SENS_LINE7,
  SENS_LINE8
};

int initRsValue[8];
int currentStatus[8];
int blackStatus[8];

int blackThreshold = 500;

enum RobotMode
{
  MAZE_MODE,
  LINE_MODE
};

RobotMode currentMode = MAZE_MODE;

// ==================== FUNCTION PROTOTYPES =======================
void driveAndAlign(int duration); // Wall following logic
void maneuver(int leftSpeed, int rightSpeed);
void robotForward(int speed);
void turnLeft();
void turnRight();
void turnAround();
void stopMotors();
void reverseSlightly();
long readDistance(int trig, int echo);
void lineSensor();
bool readLineSensor();
void StopSquare(int s1, int s2, int s3, int s4, int s5, int s6, int s7, int s8);
void physicalMaze();
void setGripperPulse(int pulseWidthMicros);
void closeGripper();
void openGripper();
void calibration();
void calibrateBlack(int threshold);
void readSensors();

void setup()
{
  Serial.begin(9600);

  // Motor Pins Setup
  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);

  // Sensor Pins Setup
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);

  // Configure Line sensor pins
  for (int i = 14; i < 22; i++)
  {
    pinMode(i, INPUT);
  }

  // Disable Encoder Pins (to avoid conflict)
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  delay(3000); // 3-second startup delay

}

void loop()
{
  if(!cal){
    openGripper();
    calibration();
  }
  closeGripper();


  distLeft = readDistance(TRIG_LEFT, ECHO_LEFT);
  distRight = readDistance(TRIG_RIGHT, ECHO_RIGHT);
  distFront = readDistance(TRIG_FRONT, ECHO_FRONT);

  if (readLineSensor())
  {
    currentMode = LINE_MODE;
  }

  if (currentMode == LINE_MODE)
  {
    lineSensor();
    return;
  }
  else
  {
    physicalMaze();
  }
}

void calibration()
{
  for (int i = 0; i < 8; i++) {
    initRsValue[i] = analogRead(SENS_ARRAY[i]);
  }
  calibrateBlack(200);
  stopMotors();
  analogWrite(MB1, 180);
  delay(700);
  stopMotors();
  cal = true;
  return;
}

void readSensors() {
  for (int i = 0; i < 8; i++) {
    currentStatus[i] = analogRead(SENS_ARRAY[i]);
  }
}

void calibrateBlack(int threshold) {
  int counter = 0;
  while (changed <= 2) {
    analogWrite(MA1, 180);
    analogWrite(MA2, 0);

    analogWrite(MB1, 170);
    analogWrite(MB2, 0);
    for (int i = 0; i < 8; i++) {
      if (currentStatus[i] >= initRsValue[i] + threshold) {
        blackStatus[i] = currentStatus[i]; 
        counter++;
        changed++;
      }
    }
    
  } 
}


void physicalMaze()
{
  // 1. STOP AND SCAN
  stopMotors();
  delay(300); // Wait for sensors to stabilize

  // === SAFETY CHECK: ANTI-STUCK ===
  if (distFront > 0 && distFront < 5)
  {
    reverseSlightly();
    return;
  }

  // === LOGIC: LEFT HAND RULE ===
  // Priority: LEFT -> FRONT -> RIGHT -> BACK

  // 1. PRIORITY: IS LEFT OPEN? -> TURN LEFT
  if (distLeft > GAP_THRESHOLD)
  {

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
  else if (distFront > WALL_STOP_DIST)
  {

    // Drive straight while correcting position (Wall Following)
    driveAndAlign(400);
  }

  // 3. PRIORITY: IS RIGHT OPEN? -> TURN RIGHT
  // (Only check if Left and Front are blocked)
  else if (distRight > GAP_THRESHOLD)
  {

    driveAndAlign(JUNCTION_ALIGN_TIME);
    stopMotors();
    delay(100);

    turnRight();

    driveAndAlign(NEW_PATH_ENTRY_TIME);
  }

  // 4. DEAD END -> TURN AROUND
  else
  {

    turnAround();
  }
}

bool readLineSensor()
{
  int s1 = analogRead(SENS_LINE1);
  int s2 = analogRead(SENS_LINE2);
  int s3 = analogRead(SENS_LINE3);
  int s4 = analogRead(SENS_LINE4);
  int s5 = analogRead(SENS_LINE5);
  int s6 = analogRead(SENS_LINE6);
  int s7 = analogRead(SENS_LINE7);
  int s8 = analogRead(SENS_LINE8);

  if (s1 >= 950 || s2 >= 950 || s3 >= 950 || s4 >= 950 || s5 >= 950 || s6 >= 950 || s7 >= 950 || s8 >= 950)
  {
    return true;
  }
  return false;
}

// ==================== MOVEMENT FUNCTIONS ====================

// This function keeps the robot centered between walls while moving forward
void driveAndAlign(int duration)
{
  unsigned long startTime = millis();

  while (millis() - startTime < (unsigned long)duration)
  {
    long currentLeft = readDistance(TRIG_LEFT, ECHO_LEFT);
    long currentRight = readDistance(TRIG_RIGHT, ECHO_RIGHT);

    // Safety: Stop immediately if a wall appears in front
    if (readDistance(TRIG_FRONT, ECHO_FRONT) < WALL_STOP_DIST)
    {
      stopMotors();
      break;
    }

    // --- WALL FOLLOWING LOGIC ---
    // If too close to LEFT wall (< 8cm) -> Steer RIGHT
    if (currentLeft < 8 && currentLeft > 0)
    {
      maneuver(BASE_SPEED + 20, BASE_SPEED - 10);
    }
    // If too close to RIGHT wall (< 8cm) -> Steer LEFT
    else if (currentRight < 8 && currentRight > 0)
    {
      maneuver(BASE_SPEED - 20, BASE_SPEED + 30);
    }
    // Otherwise -> Drive Straight with Offset
    else
    {
      maneuver(BASE_SPEED, BASE_SPEED + RIGHT_MOTOR_OFFSET);
    }

    delay(20); // Small delay for loop stability
  }
  stopMotors();
}

// 25-11-2025 14:38 the white paper shows as below 700 and the black table shows as above 950
void lineSensor()
{
  int s1 = analogRead(SENS_LINE1);
  int s2 = analogRead(SENS_LINE2);
  int s3 = analogRead(SENS_LINE3);
  int s4 = analogRead(SENS_LINE4);
  int s5 = analogRead(SENS_LINE5);
  int s6 = analogRead(SENS_LINE6);
  int s7 = analogRead(SENS_LINE7);
  int s8 = analogRead(SENS_LINE8);

  if (s1 >= blackThreshold && s2 >= blackThreshold && s3 >= blackThreshold && s4 >= blackThreshold && s5 >= blackThreshold && s6 >= blackThreshold && s7 >= blackThreshold && s8 >= blackThreshold)
  {
  }
  else if (s4 >= blackStatus[3] && s5 >= blackStatus[4])
  {
    analogWrite(MA1, 210);
    analogWrite(MA2, 0);

    analogWrite(MB1, 250);
    analogWrite(MB2, 0);
  }
  else if (s6 >= blackStatus[5])
  {
    analogWrite(MA1, 100);
    analogWrite(MA2, 0);

    analogWrite(MB1, 200);
    analogWrite(MB2, 0);
  }
  else if (s3 >= blackStatus[2])
  {
    analogWrite(MA1, 160);
    analogWrite(MA2, 0);

    analogWrite(MB1, 100);
    analogWrite(MB2, 0);
  }
  else if (s7 >= blackStatus[6])
  {
    analogWrite(MA1, 50);
    analogWrite(MA2, 0);

    analogWrite(MB1, 230);
    analogWrite(MB2, 0);
  }
  else if (s8 >= blackStatus[7])
  {
    analogWrite(MA1, 0);
    analogWrite(MA2, 0);

    analogWrite(MB1, 230);
    analogWrite(MB2, 0);
  }
  else if (s2 >= blackStatus[1])
  {
    analogWrite(MA1, 190);
    analogWrite(MA2, 0);

    analogWrite(MB1, 50);
    analogWrite(MB2, 0);
  }
  else if (s1 >= blackStatus[0])
  {
    analogWrite(MA1, 230);
    analogWrite(MA2, 0);

    analogWrite(MB1, 0);
    analogWrite(MB2, 0);
  }
  else if (s1 <= blackStatus[0] && s2 <= blackStatus[1] && s3 <= blackStatus[2] && s4 <= blackStatus[3] && s5 <= blackStatus[4] && s6 <= blackStatus[5] && s7 <= blackStatus[6] && s8 <= blackStatus[7])
  {
    currentMode = MAZE_MODE;
  }
}

void robotForward(int speed)
{
  // Motor A - Forward
  analogWrite(MA1, speed);
  analogWrite(MA2, 0);

  // Motor B - Forward (reversed direction)
  analogWrite(MB1, speed);
  analogWrite(MB2, 0);
}

void maneuver(int leftSpeed, int rightSpeed)
{
  // Constrain speeds to 0-255
  if (leftSpeed > 255)
  {
    leftSpeed = 255;
  }
  if (leftSpeed < 0)
    leftSpeed = 0;
  if (rightSpeed > 255)
    rightSpeed = 255;
  if (rightSpeed < 0)
    rightSpeed = 0;

  analogWrite(MA1, leftSpeed);
  analogWrite(MA2, 0);
  analogWrite(MB1, rightSpeed);
  analogWrite(MB2, 0);
}

void turnLeft()
{
  // Tank Turn Left
  analogWrite(MA1, 0);
  analogWrite(MA2, BASE_SPEED);
  analogWrite(MB1, BASE_SPEED);
  analogWrite(MB2, 0);
  delay(TURN_DURATION_90);
  stopMotors();
  delay(200);
}

void turnRight()
{
  // Tank Turn Right
  analogWrite(MA1, BASE_SPEED);
  analogWrite(MA2, 0);
  analogWrite(MB1, 0);
  analogWrite(MB2, BASE_SPEED);
  delay(TURN_DURATION_90);
  stopMotors();
  delay(200);
}

void turnAround()
{
  // U-Turn (Right)
  analogWrite(MA1, BASE_SPEED);
  analogWrite(MA2, 0);
  analogWrite(MB1, 0);
  analogWrite(MB2, BASE_SPEED);
  delay(TURN_DURATION_180);
  stopMotors();
  delay(200);
}

void reverseSlightly()
{
  // Back up slightly if stuck
  analogWrite(MA1, 0);
  analogWrite(MA2, BASE_SPEED);
  analogWrite(MB1, 0);
  analogWrite(MB2, BASE_SPEED + RIGHT_MOTOR_OFFSET);
  delay(350);
  stopMotors();
}

void stopMotors()
{
  digitalWrite(MA1, LOW);
  digitalWrite(MA2, LOW);
  digitalWrite(MB1, LOW);
  digitalWrite(MB2, LOW);
}

long readDistance(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 6000); // 6ms timeout (approx 1m range)

  if (duration == 0)
    return 999; // Return 999 if no echo (open space)
  return duration * 0.0343 / 2;
}

void StopSquare(int s1, int s2, int s3, int s4, int s5, int s6, int s7, int s8)
{
  while (s1 < blackThreshold && s2 >= blackThreshold && s3 >= blackThreshold && s4 >= blackThreshold && s5 >= blackThreshold && s6 >= blackThreshold && s7 >= blackThreshold && s8 >= blackThreshold)
  {
    s1 = analogRead(SENS_LINE1);
    s2 = analogRead(SENS_LINE2);
    s3 = analogRead(SENS_LINE3);
    s4 = analogRead(SENS_LINE4);
    s5 = analogRead(SENS_LINE5);
    s6 = analogRead(SENS_LINE6);
    s7 = analogRead(SENS_LINE7);
    s8 = analogRead(SENS_LINE8);
    analogWrite(MA1, 0);
    analogWrite(MA2, 0);

    analogWrite(MB1, 0);
    analogWrite(MB2, 0);
  }
}

void setGripperPulse(int pulseWidthMicros)
{
  for (int i = 0; i < 25; i++)
  {
    digitalWrite(7, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(7, LOW);
    delayMicroseconds(20000 - pulseWidthMicros);
  }
}

void closeGripper()
{
  setGripperPulse(1050);
}

void openGripper()
{
  setGripperPulse(1900);
}
