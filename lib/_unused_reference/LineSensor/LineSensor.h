/**
 * @file LineSensor.h
 * @brief Line sensor wrapper for QTR sensor array
 * 
 * Provides line detection and position tracking.
 */

#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

#include <Arduino.h>
#include <QTRSensors.h>

class LineSensor {
public:
    static const uint8_t SENSOR_COUNT = 8;

    /**
     * Initialize line sensor array
     */
    LineSensor();

    /**
     * Configure sensor pins and start (call in setup())
     * @param pins Array of analog pin numbers
     */
    void begin(const uint8_t* pins);

    /**
     * Run calibration routine
     */
    void calibrate();

    /**
     * Load pre-recorded calibration values
     * @param minValues Array of minimum values
     * @param maxValues Array of maximum values
     */
    void loadCalibration(const uint16_t* minValues, const uint16_t* maxValues);

    /**
     * Read line position (0-7000)
     * @return Position value, 3500 = centered
     */
    uint16_t readPosition();

    /**
     * Check if line is currently detected
     * @param threshold Minimum sensor value to count as line
     * @return true if any sensor sees the line
     */
    bool lineDetected(uint16_t threshold = 200);

    /**
     * Get raw sensor values (for debugging)
     * @return Pointer to sensor values array
     */
    const uint16_t* getSensorValues() const;

    /**
     * Get the underlying QTR sensor object
     * @return Reference to QTRSensors
     */
    QTRSensors& getQTR();

private:
    QTRSensors _qtr;
    uint16_t _sensorValues[SENSOR_COUNT];
    uint16_t _lastPosition;
};

#endif // LINE_SENSOR_H
