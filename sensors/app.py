from flask import Flask, request, jsonify, render_template
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
import threading
import os

# Serve templates from this sensors package directory so index.html is found
data_display = Flask(__name__, template_folder=os.path.abspath(os.path.dirname(__file__)))

# Database config (SQLite for simplicity)
data_display.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///sensor.db'
data_display.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(data_display)

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    value = db.Column(db.Float, nullable=False)
    timestamp = db.Column(db.DateTime, default=datetime.utcnow)

# Ensure tables are created within the app context (avoid circular imports)
with data_display.app_context():
    db.create_all()


@data_display.route('/sensor', methods=['POST'])
def receive_sensor_data():
    data = request.json
    value = data.get('value')

    sensor_data = SensorData(value=value)
    db.session.add(sensor_data)
    db.session.commit()

    return jsonify({"message": "Data stored successfully"})

def _simulator_loop():
    import requests, random, time
    while True:
        value = random.uniform(20, 30)  # Simulated sensor
        try:
            requests.post(
                "http://127.0.0.1:5000/sensor",
                json={"value": value},
                timeout=2
            )
        except Exception:
            pass
        time.sleep(5)


@data_display.route('/')
def index():
    data = SensorData.query.order_by(SensorData.timestamp.desc()).limit(20).all()
    return render_template('./index.html', data=data)


if __name__ == '__main__':
    sim_thread = threading.Thread(target=_simulator_loop, daemon=True)
    sim_thread.start()
    data_display.run(debug=True)
