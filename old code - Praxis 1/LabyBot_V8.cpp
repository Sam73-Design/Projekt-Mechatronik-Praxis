#include <Arduino.h>
#include <Wire.h>

#include "include/Application.h"
#include "include/Config.h"
#include "include/RobotState.h"

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setWireTimeout(1000, true);

  sensors.init();
  sensors.setAccuracyModeAll();

  pinMode(BTN_PIN, INPUT_PULLUP);  // Pin to change mode
  pinMode(LINETRACKER, INPUT);
  pinMode(LED, OUTPUT);
  attachInterrupt(0, changeMode, CHANGE);

  linetrackerCalibration();  //calibrate linetracker
  startLoop();               //start by pushing button
}

void loop() {
  //buttonInterrupt will end the loop
  while (onBlackTile || abortRun) {
    drive.stop();

    onBlackTile ? led.glow() : led.blink(100);

    if (!drive.queueEmpty()) drive.clearQueue();

    //get the robot back to the start and push button
    if (solveMode) {
      onBlackTile = false;
      led.off();
      delay(1000);
    }
    if (restartRun) {
      logic.restoreSolution();
      abortRun = false;
      restartRun = false;
      led.off();
      delay(1000);
    }
  }

  int trackerVal = analogRead(LINETRACKER);
  if (trackerVal > (lineTrackerVal - TRACKER_OFFSET)) {
    blackTile();
  }

  int direction[3];

  for (int i = 0; i < 3; i++) {
    direction[i] = sensors.getReading(i);
    Serial.print(" / ");
    Serial.print(direction[i]);
    delay(10);
  }
  Serial.println();

  if (direction[1] < 0 || direction[0] < 0 || direction[2] < 0 || direction[1] > 3000 || direction[0] > 3000 || direction[2] > 3000) {
    sensors.timeoutSensors();
    led.glow();
    return;
  }

  led.blink(1000);

  int diffSide = direction[0] - direction[2];  //used for PID
  int diffFront = lastFront - direction[1];    //used to check, if stuck
  lastFront = direction[1];

  //shortens if-statements
  //lastFront|Right|Front|Left
  //1 if path open
  byte mask = (lastFrontBit | ((direction[2] > SIDE_T) << 2) | ((direction[1] > FRONT_T) << 1) | (direction[0] > SIDE_T));
  Serial.println(mask);

  //check stuck
  if (abs(diffFront) < STUCK_DETECTION_THRESHOLD && millis() - stuckCheckTime > STUCK_INTERVALL) {
    stuckAttempts++;
    stuckCheckTime = millis();
    if (stuckAttempts >= MAX_STUCK_ATTEMPTS) {
      abortRun = true;
      return;
    }
    drive.stop();
    Serial.println("Back up");
    if (diffSide > 0) {
      drive.driveBack(BASE_SPEED * COMPENSATION_PWM + 10, BASE_SPEED - 10);
    } else {
      drive.driveBack(BASE_SPEED * COMPENSATION_PWM - 10, BASE_SPEED + 10);
    }
    delay(300);

    return;
  } else {
    stuckAttempts = 0;
  }

  drive.update(mask);

  //check motion finished
  if (drive.isBusy()) {
    return;
  }

  overHeating();

  if ((mask & (PATH_RIGHT | PATH_FRONT | PATH_LEFT)) == PATH_FRONT) {  //only front open
    Serial.println("Drive straight ");
    drive.driveStraight(diffSide);
  }
  // Find shortest path
  else if (!solveMode) {
    //drive to the middle of the crossing, if front closed
    crossingTime = millis();
    while (!drive.getIsMiddle() && (millis() - crossingTime <= 2000) && !drive.isBusy()) {
      Serial.println("Going to middle");
      direction[1] = sensors.getReading(1);
      drive.goToMiddle(direction[1]);
    }
    drive.setIsMiddle(false);
    direction[0] = sensors.getReading(0);
    direction[2] = sensors.getReading(2);

    lastFrontBit = ((direction[1] > FRONT_T) << 3);
    byte mask = (lastFrontBit | ((direction[2] > SIDE_T) << 2) | ((direction[1] > FRONT_T) << 1) | (direction[0] > SIDE_T));

    if (mask & PATH_RIGHT) {  //right open
      Serial.println("Turn right ");
      logic.rightTurnLogic();
      drive.rightTurn();
      drive.driveAhead();

    } else if (mask & PATH_FRONT) {  //front open
      Serial.println("Go ahead ");
      logic.driveAheadLogic();
      drive.driveAhead();

    } else if (mask & PATH_LEFT) {  //left open
      Serial.println("Turn left ");
      logic.leftTurnLogic();
      drive.leftTurn();
      drive.driveAhead();

    } else {  //dead end
      Serial.println("dead end ");
      logic.peekLastDirectionPath();
      drive.deadEnd();
    }
  }
  // Use shortest Path
  else if (solveMode) {
    //drive to the middle of the crossing, if front closed
    crossingTime = millis();
    while (!drive.getIsMiddle() && (millis() - crossingTime <= 2000) && !drive.isBusy()) {
      Serial.println("Going to middle");
      direction[1] = sensors.getReading(1);
      drive.goToMiddle(direction[1]);
    }
    drive.setIsMiddle(false);

    lastFrontBit = ((direction[1] > FRONT_T) << 3);
    byte mask = (lastFrontBit | ((direction[2] > SIDE_T) << 2) | ((direction[1] > FRONT_T) << 1) | (direction[0] > SIDE_T));

    logic.popLastDirectionSolution();

    switch (logic.getLastPath()) {
      //go left
      case 0:
        if (mask & PATH_LEFT) {
          drive.leftTurn();
        }
        break;
      //go straight
      case 1:
        if (mask & PATH_FRONT) {
          //drive.driveAhead(); at the end
        }
        break;
      //go right
      case 2:
        if (mask & PATH_RIGHT) {
          drive.rightTurn();
        }
        break;
      case (-2):
        abortRun = true;
        break;
    }
    drive.driveAhead();
  }
}
