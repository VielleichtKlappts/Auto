//HeadlightsControl.cpp

#include "HeadLightsControl.h"
#include "Config.h"
#include <Arduino.h>

void initHeadlights() {
    pinMode(LED_PIN, OUTPUT);
}

void headlightsOn() {
    analogWrite(LED_PIN, 255);
}

void headlightsOff() {
    analogWrite(LED_PIN, 0);
}

void controlHeadlights(int wantedBrightness) {
    int brightness = constrain(wantedBrightness, 0, 255);
    analogWrite(LED_PIN, brightness);
}