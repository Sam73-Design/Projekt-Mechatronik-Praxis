#include "../include/Application.h"
#include "../include/Config.h"
#include "../include/RobotState.h"

//Instances
ToFSensors sensors(SHT_L, SHT_F, SHT_R);
Drive drive;
StackLogic logic;
Led led;


//INPUT_PULLUP
//Switch Modes
void changeMode() {
  if (buttonPressed) {
    if (digitalRead(BTN_PIN)) {
      buttonPressed = !buttonPressed;
      if (millis() - previousMillis > DEBOUNCE_TIME) {
        if (onBlackTile) {
          solveMode = true;
        } else if (abortRun) {
          restartRun = true;
        } else {
          calibrated = true;
          stp = !stp;
        }
      }
    }
  } else {
    if (!digitalRead(BTN_PIN)) {       //Ist der Button jetzt gedrückt?
      buttonPressed = !buttonPressed;  //In Zustand 1 wechseln
      previousMillis = millis();       //Aktuelle Systemzeit speichern
    }
  }
}

//gets only called, when entering black tile
void blackTile() {
  Serial.println("Black Tile");
  drive.stop();
  onBlackTile = true;
  logic.savePath();  //save path to solution and backupSolution
}

void linetrackerCalibration() {
  delay(100);
  unsigned int val = 0;
  while (!calibrated) {
    led.blink(500);
    val = analogRead(LINETRACKER);
  }
  led.off();
  lineTrackerVal = val;
  Serial.println("Linetracker Calibrated");
  Serial.println(val);
}

//waiting for Button push
void startLoop() {
  while (!stp) {
    led.glow();
  }
  led.off();
  Serial.println("Start Loop()");
  delay(1000);
}

//let motor driver cool down a little
void overHeating() {
  if (overHeatingTime == 0) {
    overHeatingTime = millis();
    return;
  }


  if (millis() - overHeatingTime >= 10000) {
    drive.stop();
    unsigned long loop = 0;
    while (loop <= 15000) {
      loop++;
      led.blink(2000);
      delay(1);
    }
    overHeatingTime = millis();
  }
}
