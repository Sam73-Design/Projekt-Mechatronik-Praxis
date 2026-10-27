/*
Only one stepper can turn at a time
Use NEMA 17 stepper with A4988 driver or drivershield
*/
#include <Arduino.h>
#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper1 = Stepper(stepsPerRevolution, 8, 10, 9, 11);
Stepper myStepper2 = Stepper(stepsPerRevolution, 2,4,3,5);

void setup() {
    // Nothing to do (Stepper Library sets pins as outputs)
}

void loop() {
	// Rotate CW slowly at 5 RPM
	myStepper1.setSpeed(5);
  myStepper2.setSpeed(5);
	myStepper1.step(stepsPerRevolution);
  myStepper2.step(stepsPerRevolution);
	delay(1000);
	
	// Rotate CCW quickly at 10 RPM
	myStepper2.setSpeed(10);
  myStepper1.setSpeed(10);
  myStepper1.step(-stepsPerRevolution);
	myStepper2  .step(-stepsPerRevolution);
	delay(1000);
}
