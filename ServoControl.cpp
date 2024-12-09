//ServoControl.cpp

#include "ServoControl.h"
#include "Config.h"
#include <Arduino.h>
#include <Servo.h>

Servo myServo;

void initServo() {
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_BASE_POS);
}

void controlServo(int angle) {
  int target = constrain(angle, SERVO_BASE_POS - SERVO_REACH, SERVO_BASE_POS + SERVO_REACH);
  myServo.write(target);
}

