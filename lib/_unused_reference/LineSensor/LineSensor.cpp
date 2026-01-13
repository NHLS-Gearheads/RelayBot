/**
 * @file LineSensor.cpp
 * @brief Line sensor implementation
 */

#include "LineSensor.h"

LineSensor::LineSensor()
    : _lastPosition(3500)  // Start centered
{
    memset(_sensorValues, 0, sizeof(_sensorValues));
}

void LineSensor::begin(const uint8_t* pins) {
    _qtr.setTypeAnalog();
    _qtr.setSensorPins(pins, SENSOR_COUNT);
}

void LineSensor::calibrate() {
    _qtr.calibrate();
}

void LineSensor::loadCalibration(const uint16_t* minValues, const uint16_t* maxValues) {
    // Ensure calibration arrays exist
    if (_qtr.calibrationOn.minimum && _qtr.calibrationOn.maximum) {
        for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
            _qtr.calibrationOn.minimum[i] = minValues[i];
            _qtr.calibrationOn.maximum[i] = maxValues[i];
        }
    }
}

uint16_t LineSensor::readPosition() {
    _lastPosition = _qtr.readLineBlack(_sensorValues);
    return _lastPosition;
}

bool LineSensor::lineDetected(uint16_t threshold) {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        if (_sensorValues[i] > threshold) {
            return true;
        }
    }
    return false;
}

const uint16_t* LineSensor::getSensorValues() const {
    return _sensorValues;
}

QTRSensors& LineSensor::getQTR() {
    return _qtr;
}
