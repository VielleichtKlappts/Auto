//arduinor4wificar.ino

#include "MotorControl.h"
#include "ServoControl.h"
#include "HeadLightsControl.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>

#include "InternetDaten.h"

int wantedSpeed = 0;
int wantedAngle = 90;
int wantedBrightness = 0;

WiFiServer server(12345); // Server on port 12345

void setup() {
    Serial.begin(115200);
    connectToDriver();
    initCar();
}

void loop() {
    listenToDriver();
    controlCar();
}

void initCar() {
    initMotor();
    initServo();
    initHeadlights();
}

void controlCar() {
    controlMotor(wantedSpeed);
    controlServo(wantedAngle);
    controlHeadlights(wantedBrightness);
}

void connectToDriver() {
    Serial.print("Connecting to WiFi...");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.println("Server started. Waiting for connections...");
}

void listenToDriver() {
    WiFiClient client = server.available(); // Check for incoming client

    if (client) {
        Serial.println("Client connected.");

        while (client.connected()) {
            if (client.available()) {
                int key = client.read();
                int value = client.read();

                if (key == 1) {
                    wantedSpeed = value;
                    Serial.print("Updated Speed: ");
                    Serial.println(wantedSpeed);
                } else if (key == 2) {
                    wantedAngle = value;
                    Serial.print("Updated Angle: ");
                    Serial.println(wantedAngle);
                } else if (key == 3) {
                    wantedBrightness = value;
                    Serial.print("Updated Brightness: ");
                    Serial.println(wantedBrightness);
                } else {
                    Serial.println("Invalid key received.");
                }

                client.flush(); // Clear the client buffer
            }
        }

        Serial.println("Client disconnected.");
        client.stop();
    }
}
