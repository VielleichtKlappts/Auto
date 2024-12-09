// MotorControl.cpp

#include "MotorControl.h"
#include "Config.h"
#include <Arduino.h>




void initMotor() {
  pinMode(MOTOR_POWER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
}

void MotorAn(){
  digitalWrite(MOTOR_POWER_PIN, HIGH);
}
void MotorAus() {
  digitalWrite(MOTOR_POWER_PIN, LOW);
}

void controlMotor(int wantedSpeed) {
  int speed = constrain(wantedSpeed, 0, 255);

  if (speed == 0){
    MotorAus();
    analogWrite(MOTOR_PIN, speed);
  } else {
      MotorAn();
      analogWrite(MOTOR_PIN, speed);
    }
}