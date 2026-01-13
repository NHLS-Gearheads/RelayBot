/**
 * @file Ultrasonic.cpp
 * @brief HC-SR04 ultrasonic sensor implementation
 */

#include "Ultrasonic.h"

Ultrasonic::Ultrasonic(uint8_t trigPin, uint8_t echoPin)
    : _trigPin(trigPin)
    , _echoPin(echoPin)
    , _lastReadTime(0)
    , _lastDistance(999)
{
}

void Ultrasonic::begin() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    digitalWrite(_trigPin, LOW);
}

long Ultrasonic::readDistance(unsigned long timeout) {
    // Send trigger pulse
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Read echo pulse duration
    long duration = pulseIn(_echoPin, HIGH, timeout);
    
    if (duration == 0) {
        _lastDistance = 999;  // Timeout - no echo received
    } else {
        // Convert to distance: speed of sound = 343 m/s = 0.034 cm/µs
        // Distance = (duration * 0.034) / 2 (round trip)
        _lastDistance = duration * 0.034 / 2;
    }
    
    _lastReadTime = millis();
    return _lastDistance;
}

bool Ultrasonic::obstacleDetected(long thresholdCm) {
    long dist = readDistance();
    return (dist > 0 && dist < thresholdCm);
}
