#!/usr/bin/env python3
"""
Line Follower Debug UI
Real-time visualization of sensor data from the robot via Serial.
"""

import serial
import serial.tools.list_ports
from flask import Flask, render_template, jsonify
import threading
import re
import time

app = Flask(__name__)

# Shared state
state = {
    "position": 3500,
    "error": 0,
    "left_speed": 0,
    "right_speed": 0,
    "mode": "NORMAL",
    "sensors": [0] * 8,
    "sensors_normalized": [0] * 8,
    "calibration": {
        "min": [1023] * 8,
        "max": [0] * 8,
        "active": False
    },
    "raw_line": "",
    "connected": False,
    "port": ""
}

serial_lock = threading.Lock()
ser = None


def find_arduino_port():
    """Find the Arduino serial port."""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "USB" in port.description or "Arduino" in port.description or "CH340" in port.description:
            return port.device
        # Common Arduino ports on Linux
        if "/dev/ttyUSB" in port.device or "/dev/ttyACM" in port.device:
            return port.device
    return None


def parse_serial_line(line):
    """Parse a serial line and update state."""
    global state
    
    with serial_lock:
        state["raw_line"] = line.strip()
        
        # Parse position/error/speed: "pos=3500 err=0 L=150 R=150"
        match = re.search(r'pos=(\d+)\s+err=(-?\d+)\s+L=(-?\d+)\s+R=(-?\d+)', line)
        if match:
            state["position"] = int(match.group(1))
            state["error"] = int(match.group(2))
            state["left_speed"] = int(match.group(3))
            state["right_speed"] = int(match.group(4))
            state["mode"] = "NORMAL"
            return
        
        # Parse sharp turn: "SHARP pos=500 LEFT L=-200 R=200"
        match = re.search(r'SHARP pos=(\d+)\s+(LEFT|RIGHT)\s+L=(-?\d+)\s+R=(-?\d+)', line)
        if match:
            state["position"] = int(match.group(1))
            state["mode"] = f"SHARP {match.group(2)}"
            state["left_speed"] = int(match.group(3))
            state["right_speed"] = int(match.group(4))
            return
        
        # Parse lost line: "LOST! Spinning LEFT"
        if "LOST!" in line:
            state["mode"] = "LOST"
            if "LEFT" in line:
                state["mode"] = "LOST LEFT"
            elif "RIGHT" in line:
                state["mode"] = "LOST RIGHT"
            return
        
        # Parse compact sensor stream: "RAW:657,636,559,523,536,442,430,493"
        if line.strip().startswith('RAW:'):
            try:
                values = line.strip()[4:].split(',')
                for i, v in enumerate(values[:8]):
                    state["sensors"][i] = int(v)
                    # Update calibration if active
                    if state["calibration"]["active"]:
                        val = int(v)
                        if val < state["calibration"]["min"][i]:
                            state["calibration"]["min"][i] = val
                        if val > state["calibration"]["max"][i]:
                            state["calibration"]["max"][i] = val
            except:
                pass
            return
        
        # Parse sensor diagnostics: "  S0 (A0): 657" (with optional leading spaces)
        match = re.search(r'S(\d)\s*\(A\d\):\s*(\d+)', line)
        if match:
            idx = int(match.group(1))
            val = int(match.group(2))
            if 0 <= idx < 8:
                state["sensors"][idx] = val
                # Update calibration if active
                if state["calibration"]["active"]:
                    if val < state["calibration"]["min"][idx]:
                        state["calibration"]["min"][idx] = val
                    if val > state["calibration"]["max"][idx]:
                        state["calibration"]["max"][idx] = val


def serial_reader():
    """Background thread to read serial data."""
    global ser, state
    
    while True:
        try:
            port = find_arduino_port()
            if port:
                ser = serial.Serial(port, 9600, timeout=1)
                state["connected"] = True
                state["port"] = port
                print(f"Connected to {port}")
                
                while True:
                    if ser.in_waiting:
                        line = ser.readline().decode('utf-8', errors='ignore')
                        parse_serial_line(line)
            else:
                state["connected"] = False
                state["port"] = ""
                time.sleep(2)
        except Exception as e:
            print(f"Serial error: {e}")
            state["connected"] = False
            time.sleep(2)


@app.route('/')
def index():
    return render_template('debug.html')


@app.route('/api/state')
def get_state():
    with serial_lock:
        return jsonify(state)


@app.route('/api/send/<cmd>')
def send_command(cmd):
    """Send a command to the robot."""
    global ser
    try:
        if ser and ser.is_open:
            ser.write(cmd.encode())
            return jsonify({"status": "ok", "sent": cmd})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)})
    return jsonify({"status": "error", "message": "Not connected"})


@app.route('/api/calibration/start')
def start_calibration():
    """Start web-based calibration."""
    with serial_lock:
        state["calibration"]["active"] = True
        state["calibration"]["min"] = [1023] * 8
        state["calibration"]["max"] = [0] * 8
    return jsonify({"status": "ok", "message": "Calibration started"})


@app.route('/api/calibration/stop')
def stop_calibration():
    """Stop web-based calibration."""
    with serial_lock:
        state["calibration"]["active"] = False
    return jsonify({"status": "ok", "calibration": state["calibration"]})


@app.route('/api/calibration/reset')
def reset_calibration():
    """Reset calibration values."""
    with serial_lock:
        state["calibration"]["min"] = [1023] * 8
        state["calibration"]["max"] = [0] * 8
        state["calibration"]["active"] = False
    return jsonify({"status": "ok", "message": "Calibration reset"})


@app.route('/api/calibration/apply')
def apply_calibration():
    """Generate Arduino code for the calibration values."""
    with serial_lock:
        mins = state["calibration"]["min"]
        maxs = state["calibration"]["max"]
    
    code = f"""// Copy this to line_follower.cpp
static int sensorMin[LINE_SENSOR_COUNT] = {{{', '.join(map(str, mins))}}};
static int sensorMax[LINE_SENSOR_COUNT] = {{{', '.join(map(str, maxs))}}};
static bool calibrated = true;"""
    
    return jsonify({"status": "ok", "code": code, "min": mins, "max": maxs})


# Create templates directory and HTML file
import os
template_dir = os.path.join(os.path.dirname(__file__), 'templates')
os.makedirs(template_dir, exist_ok=True)

HTML_TEMPLATE = '''<!DOCTYPE html>
<html>
<head>
    <title>Line Follower Debug</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: #1a1a2e; 
            color: #eee; 
            padding: 20px;
        }
        .container { max-width: 1000px; margin: 0 auto; }
        h1 { text-align: center; margin-bottom: 20px; color: #00d4ff; }
        
        .status {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: #16213e;
            padding: 10px 20px;
            border-radius: 8px;
            margin-bottom: 20px;
        }
        .connected { color: #00ff88; }
        .disconnected { color: #ff4444; }
        
        .panel {
            background: #16213e;
            border-radius: 8px;
            padding: 20px;
            margin-bottom: 20px;
        }
        .panel h2 { 
            color: #00d4ff; 
            margin-bottom: 15px;
            font-size: 1.2em;
        }
        
        /* Calibration panel */
        .calib-status {
            display: inline-block;
            padding: 5px 15px;
            border-radius: 20px;
            font-weight: bold;
            margin-left: 10px;
        }
        .calib-status.active { background: #ff6b6b; color: #000; animation: pulse 1s infinite; }
        .calib-status.inactive { background: #333; color: #888; }
        @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.5; } }
        
        .calib-grid {
            display: grid;
            grid-template-columns: repeat(8, 1fr);
            gap: 10px;
            margin: 20px 0;
        }
        .calib-sensor {
            background: #0f0f23;
            border-radius: 8px;
            padding: 10px;
            text-align: center;
        }
        .calib-sensor .label { color: #00d4ff; font-weight: bold; margin-bottom: 10px; }
        .calib-bar-container {
            height: 150px;
            background: #222;
            border-radius: 4px;
            position: relative;
            margin: 10px 0;
        }
        .calib-bar-current {
            position: absolute;
            left: 10%;
            right: 10%;
            height: 4px;
            background: #fff;
            border-radius: 2px;
            transition: bottom 0.1s;
        }
        .calib-bar-min {
            position: absolute;
            left: 0;
            right: 0;
            height: 2px;
            background: #00ff88;
        }
        .calib-bar-max {
            position: absolute;
            left: 0;
            right: 0;
            height: 2px;
            background: #ff6b6b;
        }
        .calib-bar-range {
            position: absolute;
            left: 20%;
            right: 20%;
            background: rgba(0, 212, 255, 0.3);
        }
        .calib-values {
            font-size: 11px;
            color: #888;
        }
        .calib-values .current { color: #fff; font-size: 14px; font-weight: bold; }
        .calib-values .min { color: #00ff88; }
        .calib-values .max { color: #ff6b6b; }
        
        .calib-buttons {
            display: flex;
            gap: 10px;
            margin-top: 15px;
        }
        .calib-btn {
            padding: 12px 24px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-weight: bold;
            font-size: 14px;
        }
        .calib-btn.start { background: #00ff88; color: #000; }
        .calib-btn.stop { background: #ff6b6b; color: #000; }
        .calib-btn.reset { background: #666; color: #fff; }
        .calib-btn.apply { background: #00d4ff; color: #000; }
        .calib-btn:hover { opacity: 0.8; }
        
        .code-output {
            background: #0f0f23;
            padding: 15px;
            border-radius: 4px;
            font-family: monospace;
            font-size: 12px;
            white-space: pre-wrap;
            margin-top: 15px;
            display: none;
        }
        
        /* Sensor bar visualization */
        .sensor-bar {
            display: flex;
            justify-content: space-between;
            height: 100px;
            background: #0f0f23;
            border-radius: 8px;
            padding: 10px;
            margin-bottom: 10px;
        }
        .sensor {
            flex: 1;
            margin: 0 2px;
            background: linear-gradient(to top, #333, #333);
            border-radius: 4px;
            position: relative;
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        .sensor-fill {
            position: absolute;
            bottom: 0;
            left: 0;
            right: 0;
            background: linear-gradient(to top, #00ff88, #00d4ff);
            border-radius: 4px;
            transition: height 0.1s;
        }
        .sensor-label {
            position: absolute;
            bottom: -25px;
            font-size: 12px;
            color: #888;
        }
        .sensor-value {
            position: absolute;
            top: 5px;
            font-size: 10px;
            color: #fff;
            z-index: 1;
        }
        
        /* Position indicator */
        .position-track {
            height: 40px;
            background: #0f0f23;
            border-radius: 20px;
            position: relative;
            margin: 30px 0;
        }
        .position-marker {
            position: absolute;
            width: 20px;
            height: 40px;
            background: #ff6b6b;
            border-radius: 10px;
            top: 0;
            transform: translateX(-50%);
            transition: left 0.1s;
        }
        .position-center {
            position: absolute;
            left: 50%;
            top: 0;
            bottom: 0;
            width: 2px;
            background: #444;
        }
        
        /* Motor speeds */
        .motors {
            display: flex;
            justify-content: space-around;
            margin: 20px 0;
        }
        .motor {
            text-align: center;
            width: 120px;
        }
        .motor-bar {
            height: 150px;
            width: 40px;
            background: #0f0f23;
            border-radius: 4px;
            margin: 10px auto;
            position: relative;
            overflow: hidden;
        }
        .motor-fill {
            position: absolute;
            left: 0;
            right: 0;
            background: #00d4ff;
            transition: all 0.1s;
        }
        .motor-fill.reverse { background: #ff6b6b; }
        .motor-value { font-size: 24px; font-weight: bold; }
        
        /* Mode indicator */
        .mode {
            text-align: center;
            font-size: 28px;
            font-weight: bold;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 15px;
        }
        .mode.NORMAL { background: #1a4d1a; color: #00ff88; }
        .mode.SHARP { background: #4d4d1a; color: #ffff00; }
        .mode.LOST { background: #4d1a1a; color: #ff4444; }
        
        /* Commands */
        .commands {
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
        }
        .cmd-btn {
            padding: 10px 20px;
            background: #00d4ff;
            color: #000;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-weight: bold;
        }
        .cmd-btn:hover { background: #00a0cc; }
        
        /* Raw output */
        .raw-output {
            font-family: monospace;
            background: #0f0f23;
            padding: 10px;
            border-radius: 4px;
            font-size: 12px;
            color: #888;
            word-break: break-all;
        }
        
        /* Stats grid */
        .stats {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 10px;
            text-align: center;
        }
        .stat {
            background: #0f0f23;
            padding: 15px;
            border-radius: 8px;
        }
        .stat-value { font-size: 24px; font-weight: bold; color: #00d4ff; }
        .stat-label { font-size: 12px; color: #888; margin-top: 5px; }
        
        .instructions {
            background: #0f0f23;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 15px;
            font-size: 14px;
            line-height: 1.6;
        }
        .instructions ol { margin-left: 20px; }
        .instructions li { margin: 5px 0; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🤖 Line Follower Debug</h1>
        
        <div class="status">
            <span id="connection" class="disconnected">● Disconnected</span>
            <span id="port"></span>
        </div>
        
        <!-- CALIBRATION PANEL -->
        <div class="panel">
            <h2>🎯 Visual Calibration 
                <span id="calib-status" class="calib-status inactive">INACTIVE</span>
            </h2>
            
            <div class="instructions">
                <strong>How to calibrate:</strong>
                <ol>
                    <li>Click <strong>Start Calibration</strong></li>
                    <li>Move robot so each sensor sees the <strong>white floor</strong> (captures MIN)</li>
                    <li>Move robot so each sensor sees the <strong>black line</strong> (captures MAX)</li>
                    <li>Watch the bars fill up - green line = min, red line = max</li>
                    <li>Click <strong>Stop & Generate Code</strong> when all sensors have good range</li>
                    <li>Copy the generated code to <code>line_follower.cpp</code></li>
                </ol>
            </div>
            
            <div class="calib-grid" id="calib-grid">
                <!-- Generated by JS -->
            </div>
            
            <div class="calib-buttons">
                <button class="calib-btn start" onclick="startCalib()">▶ Start Calibration</button>
                <button class="calib-btn stop" onclick="stopCalib()">⏹ Stop & Generate Code</button>
                <button class="calib-btn reset" onclick="resetCalib()">↻ Reset</button>
            </div>
            
            <div class="code-output" id="code-output"></div>
        </div>
        
        <div class="panel">
            <div id="mode" class="mode NORMAL">NORMAL</div>
            
            <div class="stats">
                <div class="stat">
                    <div class="stat-value" id="position">3500</div>
                    <div class="stat-label">Position</div>
                </div>
                <div class="stat">
                    <div class="stat-value" id="error">0</div>
                    <div class="stat-label">Error</div>
                </div>
                <div class="stat">
                    <div class="stat-value" id="left_speed">0</div>
                    <div class="stat-label">Left Motor</div>
                </div>
                <div class="stat">
                    <div class="stat-value" id="right_speed">0</div>
                    <div class="stat-label">Right Motor</div>
                </div>
            </div>
        </div>
        
        <div class="panel">
            <h2>📊 Line Position</h2>
            <div class="position-track">
                <div class="position-center"></div>
                <div class="position-marker" id="pos-marker"></div>
            </div>
            <div style="display: flex; justify-content: space-between; color: #888; font-size: 12px;">
                <span>0 (Left)</span>
                <span>3500 (Center)</span>
                <span>7000 (Right)</span>
            </div>
        </div>
        
        <div class="panel">
            <h2>📡 Sensors (0=Left, 7=Right)</h2>
            <div class="sensor-bar" id="sensor-bar">
                <!-- Generated by JS -->
            </div>
        </div>
        
        <div class="panel">
            <h2>⚙️ Motors</h2>
            <div class="motors">
                <div class="motor">
                    <div>LEFT</div>
                    <div class="motor-bar">
                        <div class="motor-fill" id="motor-left"></div>
                    </div>
                    <div class="motor-value" id="motor-left-val">0</div>
                </div>
                <div class="motor">
                    <div>RIGHT</div>
                    <div class="motor-bar">
                        <div class="motor-fill" id="motor-right"></div>
                    </div>
                    <div class="motor-value" id="motor-right-val">0</div>
                </div>
            </div>
        </div>
        
        <div class="panel">
            <h2>🎮 Commands</h2>
            <div class="commands">
                <button class="cmd-btn" onclick="sendCmd('l')">Diagnostics (l)</button>
                <button class="cmd-btn" onclick="sendCmd('s')">Toggle Stream (s)</button>
                <button class="cmd-btn" onclick="sendCmd('p')">Toggle Polarity (p)</button>
                <button class="cmd-btn" onclick="sendCmd('1')">Kp+ (1)</button>
                <button class="cmd-btn" onclick="sendCmd('2')">Kp- (2)</button>
                <button class="cmd-btn" onclick="sendCmd('3')">Kd+ (3)</button>
                <button class="cmd-btn" onclick="sendCmd('4')">Kd- (4)</button>
                <button class="cmd-btn" onclick="sendCmd('5')">Speed+ (5)</button>
                <button class="cmd-btn" onclick="sendCmd('6')">Speed- (6)</button>
                <button class="cmd-btn" onclick="sendCmd('7')" style="background: #00ff88;">Sensitivity+ (7)</button>
                <button class="cmd-btn" onclick="sendCmd('8')" style="background: #ff6b6b;">Sensitivity- (8)</button>
                <button class="cmd-btn" onclick="sendCmd('t')">Motor Test (t)</button>
            </div>
        </div>
        
        <div class="panel">
            <h2>📜 Raw Output</h2>
            <div class="raw-output" id="raw"></div>
        </div>
    </div>
    
    <script>
        // Initialize sensor bars
        const sensorBar = document.getElementById('sensor-bar');
        for (let i = 0; i < 8; i++) {
            sensorBar.innerHTML += `
                <div class="sensor">
                    <span class="sensor-value" id="sv${i}">0</span>
                    <div class="sensor-fill" id="sf${i}"></div>
                    <span class="sensor-label">S${i}</span>
                </div>
            `;
        }
        
        // Initialize calibration grid
        const calibGrid = document.getElementById('calib-grid');
        for (let i = 0; i < 8; i++) {
            calibGrid.innerHTML += `
                <div class="calib-sensor">
                    <div class="label">S${i}</div>
                    <div class="calib-bar-container">
                        <div class="calib-bar-range" id="cr${i}"></div>
                        <div class="calib-bar-min" id="cmin${i}"></div>
                        <div class="calib-bar-max" id="cmax${i}"></div>
                        <div class="calib-bar-current" id="cc${i}"></div>
                    </div>
                    <div class="calib-values">
                        <div class="current" id="cv${i}">0</div>
                        <div><span class="min" id="cvmin${i}">1023</span> - <span class="max" id="cvmax${i}">0</span></div>
                    </div>
                </div>
            `;
        }
        
        let calibActive = false;
        
        function updateUI(data) {
            // Connection status
            const conn = document.getElementById('connection');
            if (data.connected) {
                conn.textContent = '● Connected';
                conn.className = 'connected';
            } else {
                conn.textContent = '● Disconnected';
                conn.className = 'disconnected';
            }
            document.getElementById('port').textContent = data.port;
            
            // Calibration status
            const calibStatus = document.getElementById('calib-status');
            if (data.calibration && data.calibration.active) {
                calibStatus.textContent = 'RECORDING';
                calibStatus.className = 'calib-status active';
                calibActive = true;
            } else {
                calibStatus.textContent = 'INACTIVE';
                calibStatus.className = 'calib-status inactive';
                calibActive = false;
            }
            
            // Update calibration visualization
            if (data.calibration) {
                for (let i = 0; i < 8; i++) {
                    const current = data.sensors[i];
                    const min = data.calibration.min[i];
                    const max = data.calibration.max[i];
                    
                    // Current value bar
                    const currentPercent = (current / 1023) * 100;
                    document.getElementById(`cc${i}`).style.bottom = currentPercent + '%';
                    document.getElementById(`cv${i}`).textContent = current;
                    
                    // Min/max lines
                    const minPercent = (min / 1023) * 100;
                    const maxPercent = (max / 1023) * 100;
                    document.getElementById(`cmin${i}`).style.bottom = minPercent + '%';
                    document.getElementById(`cmax${i}`).style.bottom = maxPercent + '%';
                    
                    // Range fill
                    const rangeEl = document.getElementById(`cr${i}`);
                    rangeEl.style.bottom = minPercent + '%';
                    rangeEl.style.height = (maxPercent - minPercent) + '%';
                    
                    // Values
                    document.getElementById(`cvmin${i}`).textContent = min;
                    document.getElementById(`cvmax${i}`).textContent = max;
                }
            }
            
            // Mode
            const modeEl = document.getElementById('mode');
            modeEl.textContent = data.mode;
            modeEl.className = 'mode ' + (data.mode.includes('SHARP') ? 'SHARP' : 
                                          data.mode.includes('LOST') ? 'LOST' : 'NORMAL');
            
            // Stats
            document.getElementById('position').textContent = data.position;
            document.getElementById('error').textContent = data.error;
            document.getElementById('left_speed').textContent = data.left_speed;
            document.getElementById('right_speed').textContent = data.right_speed;
            
            // Position marker (0-7000 -> 0-100%)
            const posPercent = (data.position / 7000) * 100;
            document.getElementById('pos-marker').style.left = posPercent + '%';
            
            // Sensors
            for (let i = 0; i < 8; i++) {
                const val = data.sensors[i];
                const percent = (val / 1023) * 100;
                document.getElementById(`sf${i}`).style.height = percent + '%';
                document.getElementById(`sv${i}`).textContent = val;
            }
            
            // Motors
            const maxSpeed = 255;
            const leftSpeed = data.left_speed;
            const rightSpeed = data.right_speed;
            
            const leftEl = document.getElementById('motor-left');
            const rightEl = document.getElementById('motor-right');
            
            if (leftSpeed >= 0) {
                leftEl.style.bottom = '50%';
                leftEl.style.height = (leftSpeed / maxSpeed * 50) + '%';
                leftEl.className = 'motor-fill';
            } else {
                leftEl.style.bottom = (50 + leftSpeed / maxSpeed * 50) + '%';
                leftEl.style.height = (-leftSpeed / maxSpeed * 50) + '%';
                leftEl.className = 'motor-fill reverse';
            }
            
            if (rightSpeed >= 0) {
                rightEl.style.bottom = '50%';
                rightEl.style.height = (rightSpeed / maxSpeed * 50) + '%';
                rightEl.className = 'motor-fill';
            } else {
                rightEl.style.bottom = (50 + rightSpeed / maxSpeed * 50) + '%';
                rightEl.style.height = (-rightSpeed / maxSpeed * 50) + '%';
                rightEl.className = 'motor-fill reverse';
            }
            
            document.getElementById('motor-left-val').textContent = leftSpeed;
            document.getElementById('motor-right-val').textContent = rightSpeed;
            
            // Raw output
            document.getElementById('raw').textContent = data.raw_line;
        }
        
        function sendCmd(cmd) {
            fetch('/api/send/' + cmd);
        }
        
        async function startCalib() {
            // First send 'l' to get sensor diagnostics streaming
            await fetch('/api/send/l');
            await fetch('/api/calibration/start');
        }
        
        async function stopCalib() {
            const resp = await fetch('/api/calibration/apply');
            const data = await resp.json();
            
            const codeOutput = document.getElementById('code-output');
            codeOutput.style.display = 'block';
            codeOutput.textContent = data.code;
            
            await fetch('/api/calibration/stop');
        }
        
        async function resetCalib() {
            await fetch('/api/calibration/reset');
            document.getElementById('code-output').style.display = 'none';
        }
        
        // Poll for updates
        setInterval(async () => {
            try {
                const resp = await fetch('/api/state');
                const data = await resp.json();
                updateUI(data);
            } catch (e) {}
        }, 100);
        
        // Keep sensor diagnostics streaming during calibration
        setInterval(() => {
            if (calibActive) {
                fetch('/api/send/l');
            }
        }, 500);
    </script>
</body>
</html>
'''

with open(os.path.join(template_dir, 'debug.html'), 'w') as f:
    f.write(HTML_TEMPLATE)


if __name__ == '__main__':
    # Start serial reader thread
    serial_thread = threading.Thread(target=serial_reader, daemon=True)
    serial_thread.start()
    
    print("Starting Debug UI at http://localhost:5000")
    app.run(host='0.0.0.0', port=5000, debug=False, threaded=True)
