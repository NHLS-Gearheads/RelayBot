/**
 * @file StatusLED.cpp
 * @brief Status LED implementation
 */

#include "StatusLED.h"

StatusLED::StatusLED(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin)
    : _redPin(redPin)
    , _yellowPin(yellowPin)
    , _greenPin(greenPin)
    , _currentState(RobotState::STARTUP)
    , _lastBlinkTime(0)
    , _blinkState(false)
{
}

void StatusLED::begin() {
    pinMode(_redPin, OUTPUT);
    pinMode(_yellowPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    allOff();
}

void StatusLED::setState(RobotState state) {
    _currentState = state;
    _lastBlinkTime = millis();
    _blinkState = true;

    // Set initial LED states
    switch (state) {
        case RobotState::STARTUP:
            setLEDs(true, false, false);  // Red on
            break;
        case RobotState::CALIBRATING:
            setLEDs(false, true, false);  // Yellow on
            break;
        case RobotState::READY:
        case RobotState::RUNNING:
            setLEDs(false, false, true);  // Green on
            break;
        case RobotState::SEARCHING:
            setLEDs(false, true, false);  // Yellow (will blink)
            break;
        case RobotState::ERROR:
            setLEDs(true, false, false);  // Red (will blink)
            break;
    }
}

void StatusLED::update() {
    unsigned long now = millis();
    
    // Handle blinking states
    if (_currentState == RobotState::SEARCHING || 
        _currentState == RobotState::ERROR) {
        if (now - _lastBlinkTime >= BLINK_INTERVAL) {
            _lastBlinkTime = now;
            _blinkState = !_blinkState;
            
            // Active LOW: LOW = ON, HIGH = OFF
            if (_currentState == RobotState::SEARCHING) {
                digitalWrite(_yellowPin, _blinkState ? LOW : HIGH);
            } else if (_currentState == RobotState::ERROR) {
                digitalWrite(_redPin, _blinkState ? LOW : HIGH);
            }
        }
    }
}

void StatusLED::setLEDs(bool red, bool yellow, bool green) {
    // LEDs are Active LOW (ON with LOW, OFF with HIGH) per manual section 3.5
    digitalWrite(_redPin, red ? LOW : HIGH);
    digitalWrite(_yellowPin, yellow ? LOW : HIGH);
    digitalWrite(_greenPin, green ? LOW : HIGH);
}

void StatusLED::allOff() {
    setLEDs(false, false, false);
}

void StatusLED::allOn() {
    setLEDs(true, true, true);
}
