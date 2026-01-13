/**
 * @file WirelessComm.cpp
 * @brief HC-12 Wireless Communication Implementation
 * 
 * Transmits robot telemetry data including:
 * - Motor speed and encoder ticks
 * - Robot state and line position
 * - Game status flags
 * - Sensor readings
 */

#include "WirelessComm.h"
#include "UltrasonicSensor.h"
#include "LightSensors.h"
#include "NeoPixels.h"

// SoftwareSerial for HC-12 communication
SoftwareSerial hc12Serial(HC12_RX_PIN, HC12_TX_PIN);

// HC12 library wrapper (provides better buffer management)
HC12 hc12(hc12Serial);

// Timing for periodic transmission
static unsigned long lastTransmitTime = 0;

// Buffer for incoming commands
static char cmdBuffer[32];
static int cmdIndex = 0;

void setupWireless() {
  // Enable internal pull-up on RX pin to prevent floating noise
  pinMode(HC12_RX_PIN, INPUT_PULLUP);
  
  hc12Serial.begin(9600);
  
  // Clear any garbage in the buffer
  delay(100);
  while (hc12Serial.available()) {
    hc12Serial.read();  // Flush input buffer
  }
  
  // Debug: Print to both Serial and HC12
  Serial.println(F("HC-12 Wireless Initialized"));
  Serial.print(F("HC-12 RX Pin (from HC12 TX): "));
  Serial.println(HC12_RX_PIN);
  Serial.print(F("HC-12 TX Pin (to HC12 RX): "));
  Serial.println(HC12_TX_PIN);
  
  hc12Serial.print(ROBOT_ID);
  hc12Serial.println(F(" Wireless Online"));
}

const char* getStateName(RobotState state) {
  switch (state) {
    case FOLLOW_LINE:       return "FOLLOW";
    case TURNING_LEFT:      return "TURN_L";
    case TURNING_RIGHT:     return "TURN_R";
    case TURNING_AROUND:    return "TURN180";
    case OBSTACLE_DETECTED: return "OBSTACLE";
    case AVOIDING_OBSTACLE: return "AVOIDING";
    case CHECKING_FOR_PATH_AHEAD: return "CHECKING";
    default:                return "UNKNOWN";
  }
}

const char* getLinePositionName(LinePosition pos) {
  switch (pos) {
    case T_JUNCTION:  return "T_JUNC";
    case LEFT_LINE:   return "LEFT";
    case RIGHT_LINE:  return "RIGHT";
    case NO_LINE:     return "NONE";
    case CENTER_LINE: return "CENTER";
    default:          return "UNK";
  }
}

void transmitStats() {
  unsigned long currentTime = millis();
  
  // Only transmit at specified interval
  if (currentTime - lastTransmitTime < STATS_INTERVAL) {
    return;
  }
  lastTransmitTime = currentTime;
  
  // Pause NeoPixel updates during transmission (they corrupt SoftwareSerial)
  neopixelPaused = true;
  
  // Ensure SoftwareSerial is listening (in case interrupts were disabled)
  hc12Serial.listen();
  
  // Get current distance reading
  float dist = getDistance();
  
  // Build and send stats packet
  // Format: ID|TIME|STATE|LINE|SPEED|L_TICKS|R_TICKS|DIST|FLAGS
  
  hc12Serial.print(ROBOT_ID);
  hc12Serial.print(F("|"));
  
  // Timestamp (seconds'''''''''=/*ince start)
  hc12Serial.print(currentTime / 1000);
  hc12Serial.print(F("s|"));
  
  // Robot State
  hc12Serial.print(getStateName(robotState));
  hc12Serial.print(F("|"));
  
  // Line Position
  hc12Serial.print(getLinePositionName(linePosition));
  hc12Serial.print(F("|"));
  
  // Motor Speed
  hc12Serial.print(F("SPD:"));
  hc12Serial.print(baseSpeed);
  hc12Serial.print(F("|"));
  
  // Encoder Ticks
  hc12Serial.print(F("L:"));
  hc12Serial.print(_leftTicks);
  hc12Serial.print(F(",R:"));
  hc12Serial.print(_rightTicks);
  hc12Serial.print(F("|"));
  
  // Distance to obstacle
  hc12Serial.print(F("D:"));
  if (dist == -1) {
    hc12Serial.print(F("--"));
  } else {
    hc12Serial.print((int)dist);
    hc12Serial.print(F("cm"));
  }
  hc12Serial.print(F("|"));
  
  // Game state flags (compact format)
  // C=Calibrated, P=PickedUp, G=GameStarted, D=DroppedOff, E=Ended
  hc12Serial.print(sensorsCalibrated ? F("C") : F("c"));
  hc12Serial.print(conePickedUp ? F("P") : F("p"));
  hc12Serial.print(gameStarted ? F("G") : F("g"));
  hc12Serial.print(coneDroppedOff ? F("D") : F("d"));
  hc12Serial.print(gameEnded ? F("E") : F("e"));
  
  hc12Serial.println();  // End of packet
  
  // Wait for transmission to complete, then resume NeoPixels
  hc12Serial.flush();
  neopixelPaused = false;
}

void receiveCommands() {
  // Rate limit checking to reduce SoftwareSerial/HardwareSerial conflicts
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck < 100) return;  // Only check every 100ms
  lastCheck = millis();
  
  // Call update to process HC12 library buffers
  hc12.update();
  
  while (hc12Serial.available()) {
    char c = hc12Serial.read();
    
    // Echo to Serial for debugging
    Serial.write(c);
    
    if (c == '\n' || c == '\r') {
      if (cmdIndex > 0) {
        cmdBuffer[cmdIndex] = '\0';
        
        // Process command
        // Commands format: TARGET:CMD:VALUE
        // Example: R2:SPD:200 (set R2 speed to 200)
        // Example: R2:STOP (stop R2)
        
        // Check if command is for this robot
        if (strncmp(cmdBuffer, ROBOT_ID, 2) == 0 && cmdBuffer[2] == ':') {
          char* cmd = cmdBuffer + 3;
          
          if (strncmp(cmd, "SPD:", 4) == 0) {
            int newSpeed = atoi(cmd + 4);
            if (newSpeed >= 0 && newSpeed <= 255) {
              baseSpeed = newSpeed;
              hc12Serial.print(ROBOT_ID);
              hc12Serial.print(F(" Speed set to "));
              hc12Serial.println(newSpeed);
            }
          }
          else if (strcmp(cmd, "STOP") == 0) {
            gameEnded = true;
            hc12Serial.print(ROBOT_ID);
            hc12Serial.println(F(" Emergency Stop!"));
          }
          else if (strcmp(cmd, "STATUS") == 0) {
            // Force immediate stats transmission
            lastTransmitTime = 0;
            transmitStats();
          }
        }
        
        cmdIndex = 0;
      }
    } else if (cmdIndex < 31) {
      cmdBuffer[cmdIndex++] = c;
    }
  }
}

void sendMessage(const char* message) {
  neopixelPaused = true;
  hc12Serial.listen();
  
  hc12Serial.print(ROBOT_ID);
  hc12Serial.print(F(" "));
  hc12Serial.println(message);
  
  hc12Serial.flush();
  neopixelPaused = false;
}
