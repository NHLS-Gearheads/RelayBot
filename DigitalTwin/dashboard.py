"""
Digital Twin Dashboard for RelayBot
====================================
Ingests telemetry data from Arduino R4 via /dev/ttyACM0 and displays
real-time status for robots R1, R2, and R3.

Data Format: ID|TIME|STATE|LINE|SPD:X|L:X,R:X|D:Xcm|FLAGS
Example: R2|45s|FOLLOW|CENTER|SPD:200|L:1234,R:1230|D:25cm|CPgde

Author: Mae McConigly
Date: January 2026
"""

import serial
import threading
import time
import re
from datetime import datetime
from flask import Flask, render_template, jsonify
from collections import deque

app = Flask(__name__)

# =============================================================================
# Configuration
# =============================================================================

SERIAL_PORT = '/dev/ttyACM1'
BAUD_RATE = 9600
MAX_HISTORY = 100  # Keep last 100 data points per robot

# =============================================================================
# Robot Data Storage
# =============================================================================

robots = {
    'R1': {
        'connected': False,
        'last_seen': None,
        'uptime': '0s',
        'state': 'UNKNOWN',
        'line_position': 'UNKNOWN',
        'speed': 0,
        'left_ticks': 0,
        'right_ticks': 0,
        'distance': -1,
        'flags': {
            'calibrated': False,
            'cone_picked': False,
            'game_started': False,
            'cone_dropped': False,
            'game_ended': False
        },
        'history': deque(maxlen=MAX_HISTORY),
        'messages': deque(maxlen=20)
    },
    'R2': {
        'connected': False,
        'last_seen': None,
        'uptime': '0s',
        'state': 'UNKNOWN',
        'line_position': 'UNKNOWN',
        'speed': 0,
        'left_ticks': 0,
        'right_ticks': 0,
        'distance': -1,
        'flags': {
            'calibrated': False,
            'cone_picked': False,
            'game_started': False,
            'cone_dropped': False,
            'game_ended': False
        },
        'history': deque(maxlen=MAX_HISTORY),
        'messages': deque(maxlen=20)
    },
    'R3': {
        'connected': False,
        'last_seen': None,
        'uptime': '0s',
        'state': 'UNKNOWN',
        'line_position': 'UNKNOWN',
        'speed': 0,
        'left_ticks': 0,
        'right_ticks': 0,
        'distance': -1,
        'flags': {
            'calibrated': False,
            'cone_picked': False,
            'game_started': False,
            'cone_dropped': False,
            'game_ended': False
        },
        'history': deque(maxlen=MAX_HISTORY),
        'messages': deque(maxlen=20)
    }
}

serial_connected = False
serial_error = None

# =============================================================================
# Data Parsing
# =============================================================================

def parse_telemetry(line):
    """
    Parse telemetry packet from robot.
    Format: ID|TIME|STATE|LINE|SPD:X|L:X,R:X|D:Xcm|FLAGS
    """
    try:
        parts = line.strip().split('|')
        if len(parts) < 8:
            return None
        
        robot_id = parts[0].strip()
        if robot_id not in robots:
            return None
        
        data = {
            'robot_id': robot_id,
            'uptime': parts[1].strip(),
            'state': parts[2].strip(),
            'line_position': parts[3].strip(),
            'speed': 0,
            'left_ticks': 0,
            'right_ticks': 0,
            'distance': -1,
            'flags': {}
        }
        
        # Parse speed: SPD:200
        speed_match = re.search(r'SPD:(\d+)', parts[4])
        if speed_match:
            data['speed'] = int(speed_match.group(1))
        
        # Parse encoder ticks: L:1234,R:1230
        ticks_match = re.search(r'L:(-?\d+),R:(-?\d+)', parts[5])
        if ticks_match:
            data['left_ticks'] = int(ticks_match.group(1))
            data['right_ticks'] = int(ticks_match.group(2))
        
        # Parse distance: D:25cm or D:--
        dist_match = re.search(r'D:(\d+)cm', parts[6])
        if dist_match:
            data['distance'] = int(dist_match.group(1))
        elif '--' in parts[6]:
            data['distance'] = -1
        
        # Parse flags: CPgde (uppercase = true, lowercase = false)
        if len(parts) >= 8:
            flags_str = parts[7].strip()
            if len(flags_str) >= 5:
                data['flags'] = {
                    'calibrated': flags_str[0] == 'C',
                    'cone_picked': flags_str[1] == 'P',
                    'game_started': flags_str[2] == 'G',
                    'cone_dropped': flags_str[3] == 'D',
                    'game_ended': flags_str[4] == 'E'
                }
        
        return data
        
    except Exception as e:
        print(f"Parse error: {e} for line: {line}")
        return None


def update_robot(data):
    """Update robot data store with parsed telemetry."""
    robot_id = data['robot_id']
    robot = robots[robot_id]
    
    robot['connected'] = True
    robot['last_seen'] = datetime.now()
    robot['uptime'] = data['uptime']
    robot['state'] = data['state']
    robot['line_position'] = data['line_position']
    robot['speed'] = data['speed']
    robot['left_ticks'] = data['left_ticks']
    robot['right_ticks'] = data['right_ticks']
    robot['distance'] = data['distance']
    robot['flags'] = data['flags']
    
    # Add to history for graphing
    robot['history'].append({
        'timestamp': time.time(),
        'speed': data['speed'],
        'left_ticks': data['left_ticks'],
        'right_ticks': data['right_ticks'],
        'distance': data['distance']
    })


# =============================================================================
# Dummy Data Generator for R1 and R3
# =============================================================================

import random

def generate_dummy_data():
    """Generate simulated telemetry data for R1 and R3."""
    states = ['FOLLOW_LINE', 'TURNING_LEFT', 'TURNING_RIGHT', 'AVOIDING']
    positions = ['CENTER', 'LEFT_LINE', 'RIGHT_LINE', 'T_JUNC', 'NO_LINE']
    
    start_time = time.time()
    
    while True:
        elapsed = int(time.time() - start_time)
        
        for rid in ['R1', 'R3']:
            robot = robots[rid]
            
            # Simulate connected state
            robot['connected'] = True
            robot['last_seen'] = datetime.now()
            robot['uptime'] = f"{elapsed}s"
            
            # Randomize state occasionally
            if random.random() < 0.1:
                robot['state'] = random.choice(states)
            else:
                robot['state'] = 'FOLLOW_LINE'
            
            # Randomize line position occasionally
            if random.random() < 0.15:
                robot['line_position'] = random.choice(positions)
            else:
                robot['line_position'] = 'CENTER'
            
            # Simulate speed
            robot['speed'] = random.randint(160, 200)
            
            # Simulate encoder ticks (increasing)
            robot['left_ticks'] += random.randint(5, 15)
            robot['right_ticks'] += random.randint(5, 15)
            
            # Simulate distance (occasionally close)
            if random.random() < 0.1:
                robot['distance'] = random.randint(10, 30)
            else:
                robot['distance'] = -1  # No obstacle
            
            # Simulate game progress
            if elapsed > 5:
                robot['flags']['calibrated'] = True
            if elapsed > 8:
                robot['flags']['cone_picked'] = True
            if elapsed > 10:
                robot['flags']['game_started'] = True
            
            # Add to history
            robot['history'].append({
                'timestamp': time.time(),
                'speed': robot['speed'],
                'left_ticks': robot['left_ticks'],
                'right_ticks': robot['right_ticks'],
                'distance': robot['distance']
            })
        
        time.sleep(0.5)  # Update every 500ms


# =============================================================================
# Serial Reader Thread
# =============================================================================

def serial_reader():
    """Background thread to read serial data from Arduino."""
    global serial_connected, serial_error, serial_port_obj
    
    while True:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            serial_port_obj = ser  # Store for command sending
            serial_connected = True
            serial_error = None
            print(f"Connected to {SERIAL_PORT}")
            
            try:
                while True:
                    if ser.in_waiting:
                        line = ser.readline().decode('utf-8', errors='ignore')
                        line = line.strip()
                        
                        if not line:
                            continue
                        
                        # Check if it's a telemetry packet (starts with R1, R2, or R3)
                        if line.startswith(('R1|', 'R2|', 'R3|')):
                            data = parse_telemetry(line)
                            if data:
                                update_robot(data)
                        else:
                            # It's a message - try to identify which robot
                            for rid in ['R1', 'R2', 'R3']:
                                if line.startswith(rid):
                                    robots[rid]['messages'].append({
                                        'time': datetime.now().strftime('%H:%M:%S'),
                                        'text': line
                                    })
                                    break
                            else:
                                # Unknown source, add to R2 by default
                                robots['R2']['messages'].append({
                                    'time': datetime.now().strftime('%H:%M:%S'),
                                    'text': line
                                })
                        
                        print(f"RX: {line}")
                    
                    # Check for disconnected robots (no data for 10 seconds)
                    for rid, robot in robots.items():
                        if robot['last_seen']:
                            elapsed = (datetime.now() - robot['last_seen']).total_seconds()
                            if elapsed > 10:
                                robot['connected'] = False
                    
                    time.sleep(0.01)
            finally:
                ser.close()
                serial_port_obj = None
                    
        except serial.SerialException as e:
            serial_connected = False
            serial_error = str(e)
            serial_port_obj = None
            print(f"Serial error: {e}")
            time.sleep(2)  # Wait before reconnecting
            
        except Exception as e:
            serial_connected = False
            serial_error = str(e)
            serial_port_obj = None
            print(f"Error: {e}")
            time.sleep(2)


# =============================================================================
# Flask Routes
# =============================================================================

# Serial port object for sending commands
serial_port_obj = None

@app.route('/')
def index():
    """Serve the dashboard page."""
    return render_template('dashboard.html')


@app.route('/api/status')
def api_status():
    """Return current status of all robots."""
    status = {
        'serial_connected': serial_connected,
        'serial_error': serial_error,
        'serial_port': SERIAL_PORT,
        'robots': {}
    }
    
    for rid, robot in robots.items():
        status['robots'][rid] = {
            'connected': robot['connected'],
            'last_seen': robot['last_seen'].isoformat() if robot['last_seen'] else None,
            'uptime': robot['uptime'],
            'state': robot['state'],
            'line_position': robot['line_position'],
            'speed': robot['speed'],
            'left_ticks': robot['left_ticks'],
            'right_ticks': robot['right_ticks'],
            'distance': robot['distance'],
            'flags': robot['flags'],
            'messages': list(robot['messages'])[-10:]  # Last 10 messages
        }
    
    return jsonify(status)


@app.route('/api/command', methods=['POST'])
def api_command():
    """Send a command to a robot."""
    global serial_port_obj
    
    from flask import request
    data = request.get_json()
    
    if not data:
        return jsonify({'error': 'No data provided'}), 400
    
    robot_id = data.get('robot_id', 'R2')
    command = data.get('command', '')
    
    if not command:
        return jsonify({'error': 'No command provided'}), 400
    
    # Format: ROBOT_ID:COMMAND
    full_command = f"{robot_id}:{command}\n"
    
    try:
        if serial_port_obj and serial_port_obj.is_open:
            serial_port_obj.write(full_command.encode('utf-8'))
            serial_port_obj.flush()
            print(f"TX: {full_command.strip()}")
            
            # Log the command as a message
            robots[robot_id]['messages'].append({
                'time': datetime.now().strftime('%H:%M:%S'),
                'text': f"[CMD] {command}"
            })
            
            return jsonify({'success': True, 'command': full_command.strip()})
        else:
            return jsonify({'error': 'Serial port not connected'}), 503
            
    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/start/<robot_id>', methods=['POST'])
def api_start_robot(robot_id):
    """Send START command to a specific robot."""
    global serial_port_obj
    
    if robot_id not in robots:
        return jsonify({'error': 'Unknown robot'}), 404
    
    full_command = f"{robot_id}:START\n"
    
    try:
        if serial_port_obj and serial_port_obj.is_open:
            serial_port_obj.write(full_command.encode('utf-8'))
            serial_port_obj.flush()
            print(f"TX: {full_command.strip()}")
            
            robots[robot_id]['messages'].append({
                'time': datetime.now().strftime('%H:%M:%S'),
                'text': f"[CMD] START"
            })
            
            return jsonify({'success': True, 'message': f'{robot_id} started'})
        else:
            return jsonify({'error': 'Serial port not connected'}), 503
            
    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/stop/<robot_id>', methods=['POST'])
def api_stop_robot(robot_id):
    """Send STOP command to a specific robot."""
    global serial_port_obj
    
    if robot_id not in robots:
        return jsonify({'error': 'Unknown robot'}), 404
    
    full_command = f"{robot_id}:STOP\n"
    
    try:
        if serial_port_obj and serial_port_obj.is_open:
            serial_port_obj.write(full_command.encode('utf-8'))
            serial_port_obj.flush()
            print(f"TX: {full_command.strip()}")
            
            robots[robot_id]['messages'].append({
                'time': datetime.now().strftime('%H:%M:%S'),
                'text': f"[CMD] STOP"
            })
            
            return jsonify({'success': True, 'message': f'{robot_id} stopped'})
        else:
            return jsonify({'error': 'Serial port not connected'}), 503
            
    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/start-all', methods=['POST'])
def api_start_all():
    """Send START command to all robots."""
    global serial_port_obj
    
    results = {}
    
    try:
        if serial_port_obj and serial_port_obj.is_open:
            for robot_id in ['R1', 'R2', 'R3']:
                full_command = f"{robot_id}:START\n"
                serial_port_obj.write(full_command.encode('utf-8'))
                serial_port_obj.flush()
                print(f"TX: {full_command.strip()}")
                
                robots[robot_id]['messages'].append({
                    'time': datetime.now().strftime('%H:%M:%S'),
                    'text': f"[CMD] START"
                })
                results[robot_id] = 'started'
                time.sleep(0.1)  # Small delay between commands
            
            return jsonify({'success': True, 'results': results})
        else:
            return jsonify({'error': 'Serial port not connected'}), 503
            
    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/history/<robot_id>')
def api_history(robot_id):
    """Return history data for graphing."""
    if robot_id not in robots:
        return jsonify({'error': 'Unknown robot'}), 404
    
    history = list(robots[robot_id]['history'])
    return jsonify(history)


# =============================================================================
# Main Entry Point
# =============================================================================

if __name__ == '__main__':
    # Start serial reader in background thread
    serial_thread = threading.Thread(target=serial_reader, daemon=True)
    serial_thread.start()
    
    # Start dummy data generator for R1 and R3
    dummy_thread = threading.Thread(target=generate_dummy_data, daemon=True)
    dummy_thread.start()
    
    print("=" * 60)
    print("RelayBot Digital Twin Dashboard")
    print("=" * 60)
    print(f"Serial Port: {SERIAL_PORT}")
    print(f"Baud Rate: {BAUD_RATE}")
    print("Dashboard: http://localhost:5000")
    print("Simulating: R1, R3 (dummy data)")
    print("=" * 60)
    
    # Run Flask server
    app.run(host='0.0.0.0', port=5000, debug=False, threaded=True)
