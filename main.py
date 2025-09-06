import eventlet
eventlet.monkey_patch()
from flask import Flask, render_template
from flask_socketio import SocketIO
import time
import threading
import RPi.GPIO as GPIO
import cv2

app = Flask(__name__, template_folder="template")
socketio = SocketIO(app, async_mode='eventlet')

#Hardware Configuration
PIR_PIN = 10
ULTRASONIC_TRIG = 17
ULTRASONIC_ECHO = 27
BUTTON_3RD_FLOOR = 13
BUTTON_6TH_FLOOR = 19

# Initialize GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(PIR_PIN, GPIO.IN)
GPIO.setup(ULTRASONIC_TRIG, GPIO.OUT)
GPIO.setup(ULTRASONIC_ECHO, GPIO.IN)
GPIO.setup(BUTTON_3RD_FLOOR, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(BUTTON_6TH_FLOOR, GPIO.IN, pull_up_down=GPIO.PUD_UP)

#Camera Setup
camera = cv2.VideoCapture(0, cv2.CAP_V4L2)
face_cascade = cv2.CascadeClassifier(
    cv2.data.haarcascades + 'haarcascade_frontalface_default.xml'
)

current_floor = 0
requested_floors = set()
elevator_busy = False
person_detected = False

def get_distance():
    GPIO.output(ULTRASONIC_TRIG, True)
    time.sleep(0.00001)
    GPIO.output(ULTRASONIC_TRIG, False)

    pulse_start = time.time()
    pulse_end = time.time()

    while GPIO.input(ULTRASONIC_ECHO) == 0:
        pulse_start = time.time()
    
    while GPIO.input(ULTRASONIC_ECHO) == 1:
        pulse_end = time.time()

    return (pulse_end - pulse_start) * 17150  

def detect_person():
    pir_state = GPIO.input(PIR_PIN)
    distance = get_distance()
    ret, frame = camera.read()
    faces = []
    if ret:
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        faces = face_cascade.detectMultiScale(gray, 1.1, 4)
    
    return (pir_state + (distance < 100) + len(faces)) >= 2

def elevator_control():
    global current_floor, elevator_busy, person_detected
    while True:
        if requested_floors and not elevator_busy:
            elevator_busy = True
            target = min(requested_floors, key=lambda x: abs(x - current_floor))
            
            while current_floor != target:
                current_floor += 1 if target > current_floor else -1
                socketio.emit('floor_update', current_floor)
                time.sleep(1)
            
            person_detected = detect_person()
            socketio.emit('presence', person_detected)
            
            if person_detected:
                print(f"Doors opening at floor {current_floor}")
                time.sleep(5) 
            
            requested_floors.discard(target)
            elevator_busy = False
        time.sleep(0.1)

#Web Interface
@app.route('/')
def index():
    return render_template('index.html')

@socketio.on('connect')
def handle_connect():
    socketio.emit('init', {'floor': current_floor, 'presence': person_detected})

if __name__ == '__main__':
    threading.Thread(target=elevator_control, daemon=True).start()
    
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)
