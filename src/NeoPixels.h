#ifndef NEOPIXELS_H
#define NEOPIXELS_H

#include "Config.h"

// Flag to pause NeoPixel updates during wireless TX (prevents SoftwareSerial corruption)
extern volatile bool neopixelPaused;

void setupNeoPixels();
void updateNeoPixels();
void setColor(uint8_t r, uint8_t g, uint8_t b);
void setSplitColors(uint8_t leftR, uint8_t leftG, uint8_t leftB, 
                    uint8_t rightR, uint8_t rightG, uint8_t rightB);

#endif 