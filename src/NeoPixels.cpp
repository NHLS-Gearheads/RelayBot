#include "NeoPixels.h"

// Flag to pause NeoPixel updates during wireless TX
volatile bool neopixelPaused = false;

void setupNeoPixels() {
  pixels.begin();
  pixels.setBrightness(50);
  setColor(255, 0, 0);
}

void updateNeoPixels() {
  // Skip update if paused (during wireless transmission)
  if (neopixelPaused) return;
  
  unsigned long currentMillis = millis();
  
  if (robotState == TURNING_LEFT || robotState == TURNING_RIGHT || robotState == TURNING_AROUND) {
    if (currentMillis - lastFlashTime >= flashInterval) {
      lastFlashTime = currentMillis;
      flashState = !flashState;
    }
    
    if (robotState == TURNING_LEFT) {
      if (flashState) {
        pixels.setPixelColor(0, pixels.Color(255, 165, 0));
        pixels.setPixelColor(3, pixels.Color(255, 165, 0));
        pixels.setPixelColor(1, pixels.Color(0, 0, 0));
        pixels.setPixelColor(2, pixels.Color(0, 0, 0));
      } else {
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.setPixelColor(1, pixels.Color(0, 0, 0));
        pixels.setPixelColor(2, pixels.Color(0, 0, 0));
        pixels.setPixelColor(3, pixels.Color(0, 0, 0));
      }
      pixels.show();
      delayMicroseconds(50);  // Let SoftwareSerial recover
    } else if (robotState == TURNING_RIGHT || robotState == TURNING_AROUND) {
      if (flashState) {
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.setPixelColor(3, pixels.Color(0, 0, 0));
        pixels.setPixelColor(1, pixels.Color(255, 165, 0));
        pixels.setPixelColor(2, pixels.Color(255, 165, 0));
      } else {
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.setPixelColor(1, pixels.Color(0, 0, 0));
        pixels.setPixelColor(2, pixels.Color(0, 0, 0));
        pixels.setPixelColor(3, pixels.Color(0, 0, 0));
      }
      pixels.show();
      delayMicroseconds(50);  // Let SoftwareSerial recover
    }
  } else if (robotState == FOLLOW_LINE) {
    pixels.setPixelColor(2, pixels.Color(255, 255, 255));
    pixels.setPixelColor(3, pixels.Color(255, 255, 255));
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.show();
    delayMicroseconds(50);  // Let SoftwareSerial recover
  } else {
    setColor(255, 0, 0);
  }
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for(int i=0; i<NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
  delayMicroseconds(50);  // Let SoftwareSerial recover
}

void setSplitColors(uint8_t leftR, uint8_t leftG, uint8_t leftB, 
                    uint8_t rightR, uint8_t rightG, uint8_t rightB) {
  int middle = NUM_PIXELS / 2;
  
  for(int i=0; i<middle; i++) {
    pixels.setPixelColor(i, pixels.Color(leftR, leftG, leftB));
  }
  
  for(int i=middle; i<NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(rightR, rightG, rightB));
  }
  
  pixels.show();
  delayMicroseconds(50);  // Let SoftwareSerial recover
} 