/**
 * @file StatusLED.h
 * @brief Status LED indicator library
 * 
 * Provides visual feedback using RGB/traffic light LEDs.
 */

#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>

/**
 * Robot operating states for LED indication
 */
enum class RobotState {
    STARTUP,      // Red on - initializing
    CALIBRATING,  // Yellow on - calibration in progress
    READY,        // Green on - ready to run
    RUNNING,      // Green on - normal operation
    SEARCHING,    // Yellow blinking - looking for line
    ERROR         // Red blinking - error state
};

class StatusLED {
public:
    /**
     * Initialize status LEDs
     * @param redPin Red LED pin
     * @param yellowPin Yellow LED pin
     * @param greenPin Green LED pin
     */
    StatusLED(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin);

    /**
     * Configure LED pins (call in setup())
     */
    void begin();

    /**
     * Set the current robot state (updates LEDs accordingly)
     * @param state Current robot state
     */
    void setState(RobotState state);

    /**
     * Update LED animations (call in loop() for blinking effects)
     */
    void update();

    /**
     * Manually set individual LEDs
     * @param red Red LED state
     * @param yellow Yellow LED state
     * @param green Green LED state
     */
    void setLEDs(bool red, bool yellow, bool green);

    /**
     * Turn all LEDs off
     */
    void allOff();

    /**
     * Turn all LEDs on
     */
    void allOn();

private:
    uint8_t _redPin;
    uint8_t _yellowPin;
    uint8_t _greenPin;
    RobotState _currentState;
    unsigned long _lastBlinkTime;
    bool _blinkState;
    static const unsigned long BLINK_INTERVAL = 250;
};

#endif // STATUS_LED_H
