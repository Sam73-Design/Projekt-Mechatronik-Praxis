#include <Arduino.h>
#include <AccelStepper.h>

/*
 * A4988 wiring (Arduino Uno or Mega):
 *   STEP -> D2, DIR -> D3, ENABLE -> D4 (active LOW)
 *   VDD -> 5V, logic GND -> Arduino GND
 *   RESET and SLEEP -> 5V; MS1/MS2/MS3 -> GND for full steps.
 *   VMOT -> external motor supply; motor-supply GND -> common GND.
 *   Connect each motor coil pair to 1A/1B and 2A/2B.
 * Place at least 47 uF across VMOT/GND near the driver. Set the driver's
 * current limit for your motor and carrier BEFORE use. Never connect or
 * disconnect the motor with power on. Do not power VMOT from Arduino 5V.
 *
 * Serial Monitor: 115200 baud. Motion starts only after a command.
 * f/b: continuous forward/backward, +/-: one revolution forward/backward,
 * s: immediate stop (hold position), d: disable coils (release),
 * e: enable coils, h: help. Newlines are ignored.
 * Direction depends on coil wiring; swap f/b if needed for your robot.
 */

constexpr uint8_t STEP_PIN = 2;
constexpr uint8_t DIR_PIN = 3;
constexpr uint8_t ENABLE_PIN = 4;
// Check your motor datasheet: NEMA 17 describes size, not step angle.
constexpr long FULL_STEPS_PER_REVOLUTION = 200; // Assumes a 1.8-degree motor.
constexpr long MICROSTEPS = 1; // Must match the physical MS1/MS2/MS3 wiring.
constexpr long STEPS_PER_REVOLUTION = FULL_STEPS_PER_REVOLUTION * MICROSTEPS;
constexpr float MAX_SPEED = 400.0f * MICROSTEPS; // STEP pulses per second.
constexpr float CRUISE_SPEED = 100.0f * MICROSTEPS;
constexpr float ACCELERATION = 200.0f * MICROSTEPS; // Pulses per second squared.

AccelStepper motor(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
enum class Motion { Stopped, Continuous, Position };
Motion motion = Motion::Stopped;

// Cancels any pending move. An immediate stop can lose steps at high speed.
void stopMotor() {
  motor.setCurrentPosition(motor.currentPosition());
  motion = Motion::Stopped;
}

// Signed speed: positive forward, negative backward, zero stops and holds.
// Constant-speed mode starts immediately; choose a conservative speed.
void setMotorSpeed(float stepsPerSecond) {
  stopMotor();
  motor.enableOutputs();
  motor.setSpeed(stepsPerSecond);
  motion = Motion::Continuous;
}

// Relative movement with acceleration/deceleration; returns immediately.
void moveSteps(long steps) {
  stopMotor();
  motor.enableOutputs();
  motor.move(steps);
  motion = Motion::Position;
}

void printHelp() {
  Serial.println(F("f/b: forward/reverse | +/-: one revolution"));
  Serial.println(F("s: stop and hold | d: release | e: enable | h: help"));
}

void setup() {
  // Disable the A4988 before configuring its control pins.
  digitalWrite(ENABLE_PIN, HIGH);
  pinMode(ENABLE_PIN, OUTPUT);
  motor.setEnablePin(ENABLE_PIN);
  motor.setPinsInverted(false, false, true); // ENABLE is active LOW.
  motor.disableOutputs();
  motor.setMinPulseWidth(2); // A4988 requires at least 1 us STEP high/low.
  motor.setMaxSpeed(MAX_SPEED);
  motor.setAcceleration(ACCELERATION);
  Serial.begin(115200);
  printHelp();
}

void loop() {
  if (Serial.available() > 0) {
    switch (Serial.read()) {
      case 'f': setMotorSpeed(CRUISE_SPEED); break;
      case 'b': setMotorSpeed(-CRUISE_SPEED); break;
      case '+': moveSteps(STEPS_PER_REVOLUTION); break;
      case '-': moveSteps(-STEPS_PER_REVOLUTION); break;
      case 's': stopMotor(); break;
      case 'd': stopMotor(); motor.disableOutputs(); break;
      case 'e': motor.enableOutputs(); break;
      case 'h': printHelp(); break;
      default: break;
    }
  }

  if (motion == Motion::Continuous) {
    motor.runSpeed();
  } else if (motion == Motion::Position) {
    motor.run();
    if (motor.distanceToGo() == 0) motion = Motion::Stopped;
  }

  // Later: service a second AccelStepper here, then read the line sensors.
  // Call both motors' run()/runSpeed() frequently; avoid delay() and slow I/O.
}
