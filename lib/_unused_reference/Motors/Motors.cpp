/**
 * @file Motors.cpp
 * @brief Motor driver implementation for RelayBot
 */

#include "Motors.h"

Motors::Motors(uint8_t leftPwmPin, uint8_t leftDirPin,
               uint8_t rightPwmPin, uint8_t rightDirPin)
    : _leftPwmPin(leftPwmPin)
    , _leftDirPin(leftDirPin)
    , _rightPwmPin(rightPwmPin)
    , _rightDirPin(rightDirPin)
{
}

void Motors::begin() {
    pinMode(_leftPwmPin, OUTPUT);
    pinMode(_leftDirPin, OUTPUT);
    pinMode(_rightPwmPin, OUTPUT);
    pinMode(_rightDirPin, OUTPUT);
    stop();
}

void Motors::setSpeeds(int leftSpeed, int rightSpeed) {
    setMotor(_leftPwmPin, _leftDirPin, leftSpeed);
    setMotor(_rightPwmPin, _rightDirPin, rightSpeed);
}

void Motors::stop() {
    setSpeeds(0, 0);
}

void Motors::forward(uint8_t speed) {
    setSpeeds(speed, speed);
}

void Motors::backward(uint8_t speed) {
    setSpeeds(-speed, -speed);
}

void Motors::spin(uint8_t speed, bool clockwise) {
    if (clockwise) {
        setSpeeds(speed, -speed);
    } else {
        setSpeeds(-speed, speed);
    }
}

void Motors::setMotor(uint8_t pwmPin, uint8_t dirPin, int speed) {
    // Direction inverted to match actual motor wiring
    if (speed >= 0) {
        digitalWrite(pwmPin, LOW);
        analogWrite(dirPin, speed);
    } else {
        analogWrite(pwmPin, abs(speed));
        digitalWrite(dirPin, LOW);
    }
}
