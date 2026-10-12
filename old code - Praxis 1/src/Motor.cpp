#include "../include/Motor.h"
#include "../include/Config.h"

Motor::Motor(int pinVP, int pinBP, int pinEnP) {
    pinV = pinVP;
    pinB = pinBP;
    pinEn = pinEnP;
  }

void Motor::init() {
    pinMode(pinV, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinEn, OUTPUT);
  }

void Motor::forward() {
    digitalWrite(pinV, HIGH);
    digitalWrite(pinB, LOW);
  }

void Motor::backward() {
    digitalWrite(pinV, LOW);
    digitalWrite(pinB, HIGH);
  }

void Motor::power(int value) {
    analogWrite(pinEn, value);
  }

void Motor::stop() {
    digitalWrite(pinV, LOW);
    digitalWrite(pinB, LOW);
    power(0);
  }
