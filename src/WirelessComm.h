/**
 * @file WirelessComm.h
 * @brief HC-12 Wireless Communication Module for RelayBot
 * 
 * Transmits robot stats and receives commands via HC-12 module.
 * Uses HC12 library for communication.
 */

#ifndef WIRELESSCOMM_H
#define WIRELESSCOMM_H

#include "Config.h"
#include <SoftwareSerial.h>
#include <HC12.h>

// =============================================================================
// HC-12 PIN DEFINITIONS
// =============================================================================
// SoftwareSerial(RX, TX) - First pin receives FROM HC-12, second sends TO HC-12

#define HC12_RX_PIN       9       // Arduino RX <- HC-12 TX (receive from HC-12)
#define HC12_TX_PIN       8       // Arduino TX -> HC-12 RX (send to HC-12)

// =============================================================================
// TRANSMISSION SETTINGS
// =============================================================================

#define STATS_INTERVAL    2000    // Send stats every 2 seconds (reduced frequency for testing)
#define ROBOT_ID          "R2"    // Robot identifier for multi-robot setup

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

/**
 * @brief Initialize HC-12 wireless module
 */
void setupWireless();

/**
 * @brief Transmit current robot stats
 * Call this periodically from loop()
 */
void transmitStats();

/**
 * @brief Check for and process incoming commands
 */
void receiveCommands();

/**
 * @brief Send a custom message via wireless
 * @param message The message to send
 */
void sendMessage(const char* message);

/**
 * @brief Get the state name as a string
 * @param state The robot state enum value
 * @return String representation of the state
 */
const char* getStateName(RobotState state);

/**
 * @brief Get the line position name as a string
 * @param pos The line position enum value
 * @return String representation of the position
 */
const char* getLinePositionName(LinePosition pos);

#endif
