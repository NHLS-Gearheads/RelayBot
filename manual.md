Here is the converted content of the RelayBot Manual in Markdown format.

# RelayBot Manual

**Preface**
The RelayBot is a robot designed for high-intensity racing, handmade and 3D printed with durable materials. It features sensors to outmaneuver opponents and allows owners to participate in the RelayBot community and competitions.

---

## 1. Quickstart

This section covers the PlatformIO IDE workflow and creating a "Blink" example.

### 1.1 PlatformIO IDE for VSCode

1. 
**Download & Install:** Visual Studio Code (code.visualstudio.com).


2. 
**Extension:** Open VSCode Extension Manager, search for "PlatformIO IDE", and install.



### 1.2 Setting up the project

1. Click "PlatformIO Home" on the toolbar.


2. Click "+ New Project".


3. 
**Board Selection:** Choose **Arduino Nano ATmega328** and click "Finish".


4. Open `main.cpp` in the `src` folder and replace contents with the following:



```cpp
#include "Arduino.h"

void setup(){
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
  // wait for a second
  delay(1000);
}

```

5. 
**Build:** Click the "Build" button on the PlatformIO toolbar.



---

## 2. Introduction to Arduino

### 2.1 Arduino Nano

**General Specifications:** 

* **Microcontroller:** ATmega328P
* **Operating Voltage:** 5V (Input limits: 6-20V)
* **Digital I/O:** 14 (6 PWM)
* **Analog Input:** 8
* **Current per Pin:** 20mA
* **Flash Memory:** 32 KB
* **Clock Speed:** 16 MHz

### 2.2 General Functions

* 
**`setup()`:** Runs once to configure pins and settings.


* 
**`loop()`:** Executes continuously; the heart of the code.


* 
**`delay(ms)`:** Pauses the program (blocks other actions).


* 
**`millis()`:** Returns time since start in ms; used for non-blocking timing.



2.4 Data Types 

| Type | Size/Range | Example |
| --- | --- | --- |
| `bool` | true/false | `true` |
| `byte` | 0 to 255 | `255` |
| `char` | Character | `'A'` |
| `float` | Decimal | `3.14` |
| `int` | -32,768 to 32,767 | `25` |
| `long` | -2.14 billion to +2.14 billion | `100000L` |
| `String` | Text string | `"Hello"` |

### 2.5 & 2.6 Communication and Pins

* 
**Serial:** `Serial.begin(9600)` to start, `Serial.print()` to send data, `Serial.read()` to receive .


* 
**Digital:** `pinMode(pin, MODE)`, `digitalRead(pin)`, `digitalWrite(pin, HIGH/LOW)` .


* 
**Analog:** `analogRead(pin)` (0-1023), `analogWrite(pin, value)` (PWM 0-255) .



2.8 Operators 

* **Arithmetic:** `+`, `-`, `*`, `/`, `%`
* **Comparison:** `==`, `!=`, `>`, `<`, `>=`, `<=`
* **Boolean:** `&&` (AND), `||` (OR), `!` (NOT)
* **Compound:** `+=`, `-=`, `++`, `--`

---

## 3. RelayBot Hardware

**Included Hardware:** 

* Arduino NANO & PCB Shield
* Metal chassis & Powerbank (10,000 mAh)
* 2 Motors & Driver, 2 Rotation sensors
* Gripper (Servo)
* Sensors: Ultrasonic, 8x Analog Line Follower
* Lights: 4 NeoPixels, 3 LEDs (Red, Yellow, Green)
* Inputs: 3 Pushbuttons
* Modules: Bluetooth HC05, optional ESP01/Gyro/OLED

### 3.1 & 3.2 PCB and Power

The PCB connects components to the Nano.

* 
**MAIN_PWR:** Turns electronics on/off.


* 
**MOTOR_PWR:** Turns motors on/off (prevents unexpected movement).


* 
*Note:* Switches do not control USB power from the Nano.



3.4 I/O Connectors (Pinout Map) 

| Label | Function | PCB Header | Arduino Pin |
| --- | --- | --- | --- |
| **GR** | Gripper Servo | 10-1 (1) | **D9** |
| **A6** | ADC6 | 10-1 (2) | **A6** |
| **A7** | ADC7 | 10-1 (3) | **A7** |
| **NO** | NeoPixel Data OUT | 10-1 (4) | - |
| **NI** | NeoPixel Data IN | 10-1 (5) | **D4** |
| **L1** | LED Red | 10-1 (6) | **D11** |
| **L2** | LED Yellow | 10-1 (7) | **D12** |
| **L3** | LED Green | 10-1 (8) | **D13** |
| **B1** | Button 1 | 10-2 (1) | **A0** |
| **B2** | Button 2 | 10-2 (2) | **A1** |
| **B3** | Button 3 | 10-2 (3) | **A2** |
| **ER** | ESP-Rx | 10-2 (4) | - |
| **ET** | ESP-Tx | 10-2 (5) | - |
| **EC** | ESP-CH_PD | 10-2 (6) | - |
| **HR** | HC05-Rx | 10-2 (7) | **D0** (RX) |
| **HT** | HC05-Tx | 10-2 (8) | **D1** (TX) |

### 3.5 LEDs

* 
**Active LOW:** The Red, Yellow, and Green LEDs turn **ON** with a `LOW` output and **OFF** with `HIGH`.


* **Resistors:** Current limiting resistors are calculated using Ohm's Law (). Example: Red LED (, ) on  supply requires ~470 Ohm .



**Exercises:**

1. 
**Blink:** Flash the Red LED (Pin 13/11).


2. 
**Traffic Light:** Red (3s), Green (4s), Orange (1s).


3. 
**Fade:** Use `analogWrite` and a loop to fade the LED.



### 3.6 Buttons (Switches)

* 
**Active LOW:** Inputs read `LOW` when pressed.


* 
**Pull-up Resistors:** R4-R6 pull the signal to 5V when open to prevent floating signals.



**Exercises:**

1. 
**Button Control:** Turn on Traffic Light sequence only when a button is pressed.


2. **Complex Blink:** Red flash every second; Green turns OFF if *any* button is pressed.



### 3.8 Motor Control

* 
**Motor Driver:** Extension board powering two DC motors (5V-35V).


* 
**Control:** Use PWM signals to control speed.


* 
**Sensors:** Rotation sensors provide 20 pulses per rotation.



### 3.9 Gripper (Servo)

* 
**Control:** Uses PWM pulses.


* 1.0 ms ≈ 0 degrees
* 1.5 ms ≈ 90 degrees (Neutral)
* 2.0 ms ≈ 180 degrees


* **Connection:** Signal wire connects to digital pin; Power to +5V.



### 3.10 Ultrasonic Sensor (HC-SR04)

* Uses high-frequency sound to measure distance via `Trig` (send) and `Echo` (receive) pins.



### 3.11 Line Sensor

* 8-sensor array outputs **analog** signals proportional to light intensity for precise tracking.



### 3.12 & 3.14 Wireless

* 
**Bluetooth (HC-05):** Uses 3.3V logic (requires voltage divider resistors R15-17 on the PCB). Connects to RX/TX.


* 
**Wireless Serial (HC-12):** Preprogrammed for legal frequencies/groups.



### 3.13 NeoPixels

* Addressable RGB LEDs (WS2812B).


* Chained in series; controlled via a single data line using the Adafruit NeoPixel library.



---

## 4. Testing

Use the breadboard area for prototyping without soldering. Power (+5V) and Ground (GND) headers are mounted near the breadboard for convenience.

---
