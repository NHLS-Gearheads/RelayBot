/**
 * @file Config.h
 * @brief Central configuration for RelayBot line follower
 * 
 * Contains all pin definitions, constants, and global state.
 * Modify pin values when changing physical wiring.
 * Adjust constants to tune robot behavior.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// =============================================================================
// PIN DEFINITIONS - Hardware Connections
// =============================================================================

// NeoPixel LED Strip
#define NEOPIXEL_PIN      8       // Data pin for NeoPixel strip
#define NUM_PIXELS        4       // Total number of LEDs in strip

// Line Sensor Array (QTR-8A compatible)
#define NUM_SENSORS       8       // Number of sensors in array

// Ultrasonic Sensor (HC-SR04)
#define TRIG_PIN          12      // Trigger pin
#define ECHO_PIN          13      // Echo pin

// Motor Driver (L298N or similar) - FWD/BWD swapped to match wiring
#define PIN_LEFT_FWD      10      // Left motor forward (was 11)
#define PIN_LEFT_BWD      11      // Left motor backward (was 10)
#define PIN_RIGHT_FWD     5       // Right motor forward (was 6)
#define PIN_RIGHT_BWD     6       // Right motor backward (was 5)

// Wheel Encoders
#define MOTOR_R1          3       // Left wheel encoder (interrupt capable)
// beep boop beelp boop booop
#define MOTOR_R2          2       // Right wheel encoder (interrupt capable)

// Gripper Servo
#define SERVO             9       // Servo control pin

// =============================================================================
// CONSTANTS - Array Sizes and Thresholds
// =============================================================================

#define NUM_READINGS      3       // Number of readings for object detection

// =============================================================================
// PHYSICAL PROPERTIES - Robot Measurements
// =============================================================================

extern const float WHEEL_CIRCUMFERENCE;       // Distance per wheel rotation (cm)
extern const int PULSE_PER_REVOLUTION;        // Encoder pulses per wheel rotation
extern const float DISTANCE_BETWEEN_WHEELS;   // Track width (cm)
extern const int DISTANCE_FROM_BASE_TO_CONE;  // Ticks from base to cone

// =============================================================================
// GRIPPER SETTINGS
// =============================================================================

extern const int GRIPPER_OPEN;                // Servo pulse width for open (μs)
extern const int GRIPPER_CLOSE;               // Servo pulse width for closed (μs)
extern const int pulse;                       // Default pulse duration
extern const int gripperInterval;             // Delay between servo updates (ms)

// =============================================================================
// TIMING PARAMETERS
// =============================================================================

extern const int ISR_INTERVAL;                // Min time between encoder counts (ms)
extern const unsigned long checkInterval;     // Object detection interval (ms)
extern const unsigned long flashInterval;     // LED flashing speed (ms)

// =============================================================================
// ULTRASONIC SENSOR SETTINGS
// =============================================================================

extern const int MAX_DISTANCE;                // Max ultrasonic range (cm)
extern const int OBSTACLE_THRESHOLD;          // Distance to trigger obstacle (cm)
extern const int MAX_DISTANCE_TO_CHECK;       // Upper limit for robot detection (cm)
extern const int MIN_DISTANCE_TO_CHECK;       // Lower limit for robot detection (cm)

// =============================================================================
// ENUMS - Robot State Machine
// =============================================================================

enum RobotState { 
    FOLLOW_LINE,            // Normal line following
    TURNING_LEFT,           // Executing left turn
    TURNING_RIGHT,          // Executing right turn
    TURNING_AROUND,         // 180-degree turn
    OBSTACLE_DETECTED,      // Obstacle sensed
    AVOIDING_OBSTACLE,      // Obstacle avoidance active
    CHECKING_FOR_PATH_AHEAD // Scanning for path
};

enum LinePosition { 
    T_JUNCTION,             // T-intersection detected
    LEFT_LINE,              // Line on left
    RIGHT_LINE,             // Line on right
    NO_LINE,                // No line detected
    CENTER_LINE             // Centered on line
};

// =============================================================================
// GLOBAL OBJECTS
// =============================================================================

extern Adafruit_NeoPixel pixels;              // LED control object

// =============================================================================
// GLOBAL STATE VARIABLES
// =============================================================================

extern RobotState robotState;                 // Current robot state
extern LinePosition linePosition;             // Current line position

// =============================================================================
// SENSOR VARIABLES
// =============================================================================

extern int sensorPins[NUM_SENSORS];           // Pin mappings for sensors
extern int sensorValues[NUM_SENSORS];         // Raw sensor readings
extern int sensorMin[NUM_SENSORS];            // Minimum calibration values
extern int sensorMax[NUM_SENSORS];            // Maximum calibration values
extern int sensorThreshold[NUM_SENSORS];      // Threshold for line detection

// =============================================================================
// LINE DETECTION FLAGS
// =============================================================================

extern bool leftTurn, rightTurn, tJunctionOrBase, deadEnd;

// =============================================================================
// ENCODER VARIABLES (volatile for ISR access)
// =============================================================================

extern volatile signed int _leftTicks;        // Left encoder counter
extern volatile signed int _rightTicks;       // Right encoder counter

// =============================================================================
// MOTOR CONTROL VARIABLES
// =============================================================================

extern int baseSpeed;                         // Default motor speed (0-255)

// =============================================================================
// OBJECT DETECTION VARIABLES
// =============================================================================

extern int previousTime;                      // For timing calculations
extern bool otherRobotDetected;               // Flag for robot detection
extern float distanceReadings[NUM_READINGS];  // Recent distance measurements
extern int readingCount;                      // Valid readings counter
extern int readingIndex;                      // Current position in readings array
extern unsigned long lastCheckTime;           // Last time we checked distance

// =============================================================================
// GAME STATE FLAGS
// =============================================================================

extern bool coneInSquare;                     // Cone in target square
extern bool sensorsCalibrated;                // Calibration complete
extern bool conePickedUp;                     // Cone grabbed
extern bool gameStarted;                      // Game in progress
extern bool coneDroppedOff;                   // Cone delivered
extern bool gameEnded;                        // Game complete
extern bool motionComplete;                   // Movement finished
extern bool pathChecked;                      // Path verified

// =============================================================================
// PID CONTROL VARIABLES
// =============================================================================

extern int error, lastError;                  // Current and previous error
extern float integral;                        // Integral accumulator
extern float derivative;                      // Rate of change
extern float Kp, Ki, Kd;                      // PID gains (Proportional-Integral-Derivative)
extern int correction;                        // PID output correction

// =============================================================================
// MOVEMENT CALCULATION VARIABLES
// =============================================================================

extern int pulses;                            // Encoder pulse count
extern int angle;                             // Target angle (degrees)
extern int radius;                            // Turn radius
extern int turn_Circumference;                // Turn arc length
extern float turnDistances;                   // Calculated turn distance

// =============================================================================
// LED ANIMATION VARIABLES
// =============================================================================

extern unsigned long lastFlashTime;           // Last LED state change
extern bool flashState;                       // Current flash state

#endif