#pragma once

#include <Arduino.h>

class Motor {
private:
  int pinV;   //Forward pin
  int pinB;   //Backward pin
  int pinEn;  //Enable Pin; PWM

public:
  Motor(int pinVP, int pinBP, int pinEnP);

  void init();

  void forward();

  void backward();

  void power(int value);

  void stop();
};
