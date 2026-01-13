/**
 * @file Motors.h
 * @brief Motor driver library for RelayBot
 * 
 * Provides motor control abstraction for differential drive.
 */

#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

class Motors {
public:
    /**
     * Initialize motor driver with pin assignments
     * @param leftPwmPin PWM pin for left motor speed
     * @param leftDirPin Direction pin for left motor
     * @param rightPwmPin PWM pin for right motor speed
     * @param rightDirPin Direction pin for right motor
     */
    Motors(uint8_t leftPwmPin, uint8_t leftDirPin, 
           uint8_t rightPwmPin, uint8_t rightDirPin);

    /**
     * Initialize motor pins (call in setup())
     */
    void begin();

    /**
     * Set motor speeds
     * @param leftSpeed Left motor speed (-255 to 255)
     * @param rightSpeed Right motor speed (-255 to 255)
     */
    void setSpeeds(int leftSpeed, int rightSpeed);

    /**
     * Stop both motors immediately
     */
    void stop();

    /**
     * Drive forward at specified speed
     * @param speed Speed (0-255)
     */
    void forward(uint8_t speed);

    /**
     * Drive backward at specified speed
     * @param speed Speed (0-255)
     */
    void backward(uint8_t speed);

    /**
     * Spin in place (left motor backward, right forward)
     * @param speed Speed (0-255)
     * @param clockwise Direction of spin
     */
    void spin(uint8_t speed, bool clockwise);

private:
    uint8_t _leftPwmPin;
    uint8_t _leftDirPin;
    uint8_t _rightPwmPin;
    uint8_t _rightDirPin;

    void setMotor(uint8_t pwmPin, uint8_t dirPin, int speed);
};

#endif // MOTORS_H
