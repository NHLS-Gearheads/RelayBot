/**
 * @file Ultrasonic.h
 * @brief HC-SR04 ultrasonic distance sensor driver
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class Ultrasonic {
public:
    /**
     * Initialize ultrasonic sensor
     * @param trigPin Trigger pin
     * @param echoPin Echo pin
     */
    Ultrasonic(uint8_t trigPin, uint8_t echoPin);

    /**
     * Configure pins (call in setup())
     */
    void begin();

    /**
     * Read distance in centimeters
     * @param timeout Maximum pulse wait time in microseconds
     * @return Distance in cm, or 999 if timeout
     */
    long readDistance(unsigned long timeout = 30000);

    /**
     * Check if obstacle is within threshold
     * @param thresholdCm Distance threshold in cm
     * @return true if obstacle detected closer than threshold
     */
    bool obstacleDetected(long thresholdCm);

private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    unsigned long _lastReadTime;
    long _lastDistance;
};

#endif // ULTRASONIC_H
