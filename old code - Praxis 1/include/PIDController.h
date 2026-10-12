#pragma once

#include <Arduino.h>

class PIDController {
private:
  float kp;  // Proportional gain
  float ki;  // Integral gain
  float kd;  // Derivative gain

  float lastError;
  float integral;
  float integralMax;  // Anti-Windup limit

  unsigned long lastTime;

public:
  PIDController(float p, float i, float d, float intMax = 100.0);

  // compute value based on error
  float compute(float setpoint, float measured);

  //reset controller
  void reset();

  void printGains();
};
