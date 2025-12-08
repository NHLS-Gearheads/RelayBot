#ifndef BOT_CONFIG_H
#define BOT_CONFIG_H

// Motor pins
#define MOTOR_A_IN1 11  // Direction control 1 (PWM)
#define MOTOR_A_IN2 10  // Direction control 2 (PWM)

#define MOTOR_B_IN3 9   // Direction control 1 (PWM)
#define MOTOR_B_IN4 3   // Direction control 2 (PWM)

// Ultrasonic sensor pins
#define ULTRA_TRIG 4   // Trig pin
#define ULTRA_ECHO 12  // Echo pin

// Rotation sensors (Encoders)
#define SENSOR_PIN_LEFT  2   // Left wheel encoder
#define SENSOR_PIN_RIGHT 13  // Right wheel encoder

// Delays
#define METER_TIME 2500
#define TURN_180_TIME 900
#define TURN_90_TIME 730

// --- ROBOT DIMENSIONS & CALIBRATION ---
#define WHEEL_DIAMETER_MM   65.0
#define TICKS_PER_ROTATION  30.0
#define TRACK_WIDTH_MM      135.0

// --- LINE SENSOR (MD0167) ---
// A0 = IR Enable, A1-A7 = D1-D7 (7 data pins)
#define IR_ENABLE_PIN       A0
#define IR_PIN_1            A1  // D1 - Far Right
#define IR_PIN_2            A2  // D2
#define IR_PIN_3            A3  // D3
#define IR_PIN_4            A4  // D4 - Center
#define IR_PIN_5            A5  // D5
#define IR_PIN_6            A6  // D6 (Analog only)
#define IR_PIN_7            A7  // D7 - Far Left (Analog only)

// Thresholds
#define ENCODER_THRESHOLD   500
#define THRESHOLD           500 // Line detection threshold

// AUTOMATIC CALCULATION:
#define TICKS_PER_DEGREE    (((TRACK_WIDTH_MM / WHEEL_DIAMETER_MM) * TICKS_PER_ROTATION) / 360.0)

#endif