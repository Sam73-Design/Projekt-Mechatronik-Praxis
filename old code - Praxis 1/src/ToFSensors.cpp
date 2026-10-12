#include "../include/ToFSensors.h"
#include "../include/Config.h"

ToFSensors::ToFSensors(uint8_t shtPinLeftP, uint8_t shtPinFrontP, uint8_t shtPinRightP) {
    shtPin[0] = shtPinLeftP;
    shtPin[1] = shtPinFrontP;
    shtPin[2] = shtPinRightP;
  }

void ToFSensors::init() {
    for (int i = 0; i < 3; i++) {
      pinMode(shtPin[i], OUTPUT);
      digitalWrite(shtPin[i], LOW);
    }
    delay(10);

    for (int i = 0; i < 3; i++) {
      //assign each sensor a unique address
      digitalWrite(shtPin[i], HIGH);
      delay(10);
      if (!L0X[i].init()) {  //initialize sensor 1
        //Add error LED
        Serial.print("Fehler in Sensor ");
        Serial.println(i);
        while (1)  //infinit while loop
          ;
      }
      L0X[i].setAddress(address[i]);
      L0X[i].setTimeout(500);
      L0X[i].startContinuous();
    }
  }

void ToFSensors::setAccuracyModeAll() {
    for (int i = 0; i < 3; i++) {
      L0X[i].setMeasurementTimingBudget(200000);
    }
  }

void ToFSensors::setSpeedModeAll() {
    for (int i = 0; i < 3; i++) {
      L0X[i].setMeasurementTimingBudget(20000);
    }
  }

void ToFSensors::setShtAll(boolean state) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(shtPin[i], state);
    }
  }

void ToFSensors::setShtSingle(boolean state, uint8_t sht) {
    digitalWrite(shtPin[sht], state);
  }

int ToFSensors::getReading(int direction) {
    switch (direction) {
      case 0:
        return (L0X[direction].readRangeContinuousMillimeters() - L0X_COMP_L);
        break;
      case 1:
        return L0X[direction].readRangeContinuousMillimeters();
        break;
      case 2:
        return L0X[direction].readRangeContinuousMillimeters() - L0X_COMP_R;
        break;
      default:
        return -1;
        break;
    }
  }

void ToFSensors::timeoutSensors() {
    for (int i = 0; i < 3; i++) {
      if (L0X[i].timeoutOccurred()) {
        Serial.print("Timeout sensor ");
        Serial.println(i);
      }
    }
  }
