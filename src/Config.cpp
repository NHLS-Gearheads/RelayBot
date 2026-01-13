/**
 * @file Config.cpp
 * @brief Configuration implementation for RelayBot
 * 
 * Defines constant values and initializes global variables.
 */

#include "Config.h"

// =============================================================================
// PHYSICAL PROPERTIES
// =============================================================================

const float WHEEL_CIRCUMFERENCE = 20.4;       // Wheel perimeter (cm)
const int PULSE_PER_REVOLUTION = 20;          // Encoder ticks per rotation
const float DISTANCE_BETWEEN_WHEELS = 22.75;  // Track width (cm)
const int DISTANCE_FROM_BASE_TO_CONE = 33;    // Distance in encoder ticks

// =============================================================================
// GRIPPER SETTINGS
// =============================================================================

const int GRIPPER_OPEN = 1750;                // Open position (μs)
const int GRIPPER_CLOSE = 1000;               // Closed position (μs)
const int pulse = 2000;                       // Default pulse width (μs)
const int gripperInterval = 20;               // Servo update interval (ms)

// =============================================================================
// TIMING PARAMETERS
// =============================================================================

const int ISR_INTERVAL = 20;                  // Encoder debounce (ms)
const unsigned long checkInterval = 100;      // Object detection interval (ms)
const unsigned long flashInterval = 100;      // LED blink rate (ms)

// =============================================================================
// ULTRASONIC SENSOR SETTINGS
// =============================================================================

const int MAX_DISTANCE = 50;                  // Maximum range (cm)
const int OBSTACLE_THRESHOLD = 15;            // Obstacle distance (cm)
const int MAX_DISTANCE_TO_CHECK = 30;         // Max robot detection (cm)
const int MIN_DISTANCE_TO_CHECK = 5;          // Min robot detection (cm)

// =============================================================================
// GLOBAL OBJECTS
// =============================================================================

Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

// =============================================================================
// GLOBAL STATE VARIABLES
// =============================================================================

RobotState robotState = FOLLOW_LINE;
LinePosition linePosition = CENTER_LINE;

// =============================================================================
// SENSOR VARIABLES
// =============================================================================

int sensorPins[NUM_SENSORS] = {A0, A1, A2, A3, A4, A5, A6, A7};
int sensorValues[NUM_SENSORS];
int sensorMin[NUM_SENSORS];
int sensorMax[NUM_SENSORS];
int sensorThreshold[NUM_SENSORS];

// =============================================================================
// LINE DETECTION FLAGS
// =============================================================================

bool leftTurn = false;
bool rightTurn = false;
bool tJunctionOrBase = false;
bool deadEnd = false;

// =============================================================================
// ENCODER VARIABLES
// =============================================================================

volatile signed int _leftTicks = 8;
volatile signed int _rightTicks = 0;

// =============================================================================
// MOTOR CONTROL VARIABLES
// =============================================================================

int baseSpeed = 180;  // Reduced from 255 to allow PID corrections

// =============================================================================
// OBJECT DETECTION VARIABLES
// =============================================================================

int previousTime = 0;
bool otherRobotDetected = false;
float distanceReadings[NUM_READINGS];

int readingCount = 0;
int readingIndex = 0;
unsigned long lastCheckTime = 0;

// =============================================================================
// GAME STATE FLAGS
// =============================================================================

bool coneInSquare = true;
bool sensorsCalibrated = false;
bool conePickedUp = false;
bool gameStarted = false;
bool coneDroppedOff = false;
bool gameEnded = false;
bool motionComplete = true;
bool pathChecked = false;

// =============================================================================
// PID CONTROL VARIABLES
// =============================================================================

int error = 0;
int lastError = 0;
float integral = 0;
float derivative = 0;
float Kp, Ki, Kd;
int correction;

// =============================================================================
// MOVEMENT CALCULATION VARIABLES
// =============================================================================

int pulses;
int angle;
int radius = DISTANCE_BETWEEN_WHEELS;
int turn_Circumference = 2 * 3.14 * radius;
float turnDistances = 0;

// =============================================================================
// LED ANIMATION VARIABLES
// =============================================================================

unsigned long lastFlashTime = 0;
bool flashState = false;