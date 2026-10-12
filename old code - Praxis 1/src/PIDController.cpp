#include "../include/PIDController.h"
#include "../include/Config.h"

PIDController::PIDController(float p, float i, float d, float intMax)
    : kp(p), ki(i), kd(d), lastError(0), integral(0), integralMax(intMax), lastTime(0) {}

float PIDController::compute(float setpoint, float measured) {
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;

    // first call or long pause
    if (lastTime == 0 || dt > 1.0) {
      lastTime = now;
      lastError = 0;
      integral = 0;
      return 0;
    }
    lastTime = now;

    float error = setpoint - measured;

    // Integral (with Anti-Windup)
    integral += error * dt;
    integral = constrain(integral, -integralMax, integralMax);

    // Derivative
    float derivative = (error - lastError) / dt;
    lastError = error;

    // PID-formula
    float output = kp * error + ki * integral + kd * derivative;
    return output;
  }

void PIDController::reset() {
    lastError = 0;
    integral = 0;
    lastTime = 0;
  }

void PIDController::printGains() {
    Serial.print(" \\ LastError: ");
    Serial.print(lastError);
    Serial.print(" \\ Integral: ");
    Serial.println(integral);
  }
