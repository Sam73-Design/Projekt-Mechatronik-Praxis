#include <Arduino.h>
#include <Stepper.h>
Stepper myStepper(64,10,11,12,13); // 32 steps, pins 10, 11, 12, 13
int steps = 2038; // number of steps to move

//Function must be declared before it is used in the loop function

void setup() {
  // put your setup code here, to run once:
  myStepper.setSpeed(200); // set the speed to 60 RPM
}

void loop() {
  // put your main code here, to run repeatedly:
  myStepper.step(steps); // move the motor 2038 steps
  steps = -steps; // reverse the direction of the next movement
}
