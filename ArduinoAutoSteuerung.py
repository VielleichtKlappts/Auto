#ArduinoAutoSteuerung.py

import tkinter as tk
import socket
import requests
from PIL import Image, ImageTk
from io import BytesIO
import threading
import time



# window size
windowHeight = 800
windowWidth = 1460

#Communication
arduinoCommunicationInterval = 100
ARDUINO_IP = "192.168.x.x"
ARDUINO_PORT = 12345

ESP32_CAM_IP = "http://192.168.x.x/stream"


# speed
speedMin = 0
speedMax = 255


# brightness
lightMin = 0
lightMax = 255


# steering
servoMin = 50
servoMax = 130
# damit die reifen geradeaus zeigen muss man in die entgegengesetzte richtung übersteuern
minToStraight = 100
maxToStraight = 65





class CarControllerApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Car Controller")
        self.master.geometry(f"{windowWidth}x{windowHeight}")

        # frame for the controls
        self.controls_frame = tk.Frame(master)
        self.controls_frame.pack(side=tk.BOTTOM, anchor='sw', padx=  20, pady= 20)

        # Speed Control
        self.speed_label = tk.Label(self.controls_frame, text="Speed")
        self.speed_label.pack()
        self.speed_slider = tk.Scale(self.controls_frame, from_=speedMin, to=speedMax, orient=tk.HORIZONTAL)
        self.speed_slider.pack()

        # Angle Control
        self.angle_label = tk.Label(self.controls_frame, text="Angle")
        self.angle_label.pack()
        self.angle_slider = tk.Scale(self.controls_frame, from_=servoMin, to=servoMax, orient=tk.HORIZONTAL)
        self.angle_slider.pack()

        # Brightness Control
        self.brightness_label = tk.Label(self.controls_frame, text="Brightness")
        self.brightness_label.pack()
        self.brightness_slider = tk.Scale(self.controls_frame, from_=lightMin, to=lightMax, orient=tk.HORIZONTAL)
        self.brightness_slider.pack()

        # Status Labels
        self.arduino_status_label = tk.Label(self.controls_frame, text="Arduino: ●", fg="red")
        self.arduino_status_label.pack()
        self.esp32_status_label = tk.Label(self.controls_frame, text="ESP32-CAM: ●", fg="red")
        self.esp32_status_label.pack()

        # Video Frame Placeholder
        self.video_label = tk.Label(self.master)
        self.video_label.pack(fill=tk.BOTH, expand=True)

        # Key press tracking
        self.key_pressed = {}

        # Arduino connection state
        self.arduino_connected = False

        # Bind key events
        self.master.bind("<KeyPress>", self.on_key_press)
        self.master.bind("<KeyRelease>", self.on_key_release)

        # Initialize the controls state
        self.speed = self.speed_slider.get()
        self.angle = self.angle_slider.get()
        self.brightness = self.brightness_slider.get()

        # Start background tasks
        threading.Thread(target=self.check_arduino_connection, daemon=True).start()
        threading.Thread(target=self.update_video_stream, daemon=True).start()

        # Start sending controls periodically
        self.send_controls_periodically()

    def send_controls(self):
        try:
            # Connect to Arduino and send the updated controls
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.connect((ARDUINO_IP, ARDUINO_PORT))
                s.sendall(bytes([1, self.speed]))  # Speed command
                s.sendall(bytes([2, self.angle]))  # Angle command
                s.sendall(bytes([3, self.brightness]))  # Brightness command
        except Exception:
            pass  # Ignore connection errors

    def send_controls_periodically(self):
        # Check if any of the control values have changed and send them
        if (self.speed != self.speed_slider.get() or 
            self.angle != self.angle_slider.get() or 
            self.brightness != self.brightness_slider.get()):

            self.speed = self.speed_slider.get()
            self.angle = self.angle_slider.get()
            self.brightness = self.brightness_slider.get()

            if self.arduino_connected:
                self.send_controls()

        # Call this function again after 100 milliseconds
        self.master.after(arduinoCommunicationInterval, self.send_controls_periodically)

    def check_arduino_connection(self):
        while True:
            try:
                # Try connecting to Arduino
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.settimeout(1)
                    s.connect((ARDUINO_IP, ARDUINO_PORT))
                self.arduino_status_label.config(text="Arduino: ●", fg="green")
                self.arduino_connected = True
            except Exception:
                self.arduino_status_label.config(text="Arduino: ●", fg="red")
                self.arduino_connected = False
            time.sleep(2)  # Check every 2 seconds

    def update_video_stream(self):
        while True:
            try:
                # Open a connection to the ESP32-CAM stream
                stream = requests.get(ESP32_CAM_IP, stream=True, timeout=5)
                if stream.status_code == 200:
                    self.esp32_status_label.config(text="ESP32-CAM: ●", fg="green")
                    bytes_data = b""
                    for chunk in stream.iter_content(chunk_size=1024):
                        bytes_data += chunk
                        a = bytes_data.find(b'\xff\xd8')  # JPEG start
                        b = bytes_data.find(b'\xff\xd9')  # JPEG end
                        if a != -1 and b != -1:
                            # Extract JPEG frame
                            jpg = bytes_data[a:b+2]
                            bytes_data = bytes_data[b+2:]

                            img = Image.open(BytesIO(jpg))
                            img = img.resize((windowWidth, windowHeight), Image.Resampling.LANCZOS)  # Resize to fit window
                            self.video_image = ImageTk.PhotoImage(img)

                            # Update the video label to fill the entire window (background layer)
                            self.video_label.config(image=self.video_image)
                            self.video_label.place(relwidth=1, relheight=1, relx=0, rely=0)  # Fill the window completely
                            # Set the background video as the lowest widget (behind controls)
                            self.video_label.lower()

                else:
                    self.esp32_status_label.config(text="ESP32-CAM: ●", fg="red")

            except Exception as e:
                print(f"Error: {e}")
                self.esp32_status_label.config(text="ESP32-CAM: ●", fg="red")

            time.sleep(0.05)  # Small delay to reduce CPU usage


    def on_key_press(self, event):
        """Handle key press events"""
        if event.keysym == "w":
            self.speed_slider.set(min(self.speed_slider.get() + 25, 255))
        elif event.keysym == "s":
            self.speed_slider.set(max(self.speed_slider.get() - 25, 0))
        elif event.keysym == "r":
            self.brightness_slider.set(min(self.brightness_slider.get() + 25, 255))
        elif event.keysym == "f":
            self.brightness_slider.set(max(self.brightness_slider.get() - 25, 0))
        elif event.keysym == "d" and not self.key_pressed.get("d", False):
            self.angle_slider.set(50)
            self.key_pressed["d"] = True
        elif event.keysym == "a" and not self.key_pressed.get("a", False):
            self.angle_slider.set(130)
            self.key_pressed["a"] = True
        elif event.keysym == "q":
            self.speed_slider.set(255)
        elif event.keysym == "e":
            self.speed_slider.set(0)

    def on_key_release(self, event):
        """Handle key release events"""
        if event.keysym == "a":
            self.angle_slider.set(maxToStraight)
            self.key_pressed["a"] = False
        elif event.keysym == "d":
            self.angle_slider.set(minToStraight)
            self.key_pressed["d"] = False

if __name__ == "__main__":
    root = tk.Tk()
    app = CarControllerApp(root)
    root.mainloop()
