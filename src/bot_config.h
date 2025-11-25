#ifndef BOT_CONFIG_H
#define BOT_CONFIG_H

// Motor pins
#define MOTOR_A_IN1 10  // Direction control 1 (PWM)
#define MOTOR_A_IN2 11  // Direction control 2 (PWM)

#define MOTOR_B_IN3 3   // Direction control 1 (PWM)
#define MOTOR_B_IN4 9   // Direction control 2 (PWM)

// Ultrasonic sensor pins
#define ULTRA_TRIG 4  // Trig pin
#define ULTRA_ECHO 12  // Echo pin

// Rotation sensors
#define SENSOR_PIN_RIGHT A0 
#define SENSOR_PIN_LEFT A1

// Delays
#define METER_TIME 2500
#define TURN_180_TIME 900
#define TURN_90_TIME 730

// --- ROBOT DIMENSIONS & CALIBRATION ---
#define WHEEL_DIAMETER_MM   65.0
#define TICKS_PER_ROTATION  30.0
#define TRACK_WIDTH_MM      135.0

// AUTOMATIC CALCULATION:
// Note: We use parentheses ( ... ) to ensure the math happens correctly
#define TICKS_PER_DEGREE    (((TRACK_WIDTH_MM / WHEEL_DIAMETER_MM) * TICKS_PER_ROTATION) / 360.0)

#define THRESHOLD           500

#endif