# RelayBot

A line-following robot built with Arduino Nano, using PD control for smooth navigation.

## 🏗️ Project Structure

```
RelayBot/
├── platformio.ini          # PlatformIO configuration
├── include/                 # Configuration headers
│   ├── pins.h              # Hardware pin definitions
│   └── config.h            # Tuning parameters & constants
├── lib/                     # Custom libraries
│   ├── Motors/             # Motor driver (L298N compatible)
│   ├── LineSensor/         # QTR sensor array wrapper
│   ├── Ultrasonic/         # HC-SR04 distance sensor
│   └── StatusLED/          # LED indicator system
├── src/
│   └── main.cpp            # Main application
├── DigitalTwin/            # Debug & visualization tools
│   ├── debug_ui.py         # Flask-based debug server
│   ├── templates/          # Web UI templates
│   └── webusb_debug.html   # Standalone WebUSB debug
└── test/                   # Unit tests
```

## 🔧 Hardware

| Component | Pin(s) |
|-----------|--------|
| Left Motor (PWM/DIR) | 11, 10 |
| Right Motor (PWM/DIR) | 3, 9 |
| QTR-8A Sensor Array | A0-A7 |
| Ultrasonic (Trig/Echo) | 4, 12 |
| Status LEDs (R/Y/G) | 6, 7, 8 |

## ⚙️ Configuration

Edit `include/config.h` to tune:
- **PID gains**: `PID_KP`, `PID_KD`
- **Motor speeds**: `SPEED_BASE`, `SPEED_MAX`, `SPEED_TURN`
- **Timing**: Debounce, recovery timeouts
- **Calibration**: Pre-recorded sensor min/max values

Edit `include/pins.h` to change wiring.

## 🚀 Quick Start

```bash
# Build
pio run

# Upload to robot
pio run --target upload

# Monitor serial output
pio device monitor
```

## 🔬 Debugging

The `DigitalTwin/` folder contains visualization tools:

```bash
cd DigitalTwin
pip install -r requirements.txt
python debug_ui.py
```

Open `http://localhost:5000` to see real-time sensor data.

## 📝 License

NHLS Gearheads © 2026
