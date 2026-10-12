#pragma once

#include <Arduino.h>
#include <VL53L0X.h>

class ToFSensors {

  uint8_t shtPin[3];
  uint8_t address[3] = { 0x30, 0x31, 0x32 };
  VL53L0X L0X[3] = { VL53L0X(), VL53L0X(), VL53L0X() };  //left, front, right

public:
  ToFSensors(uint8_t shtPinLeftP, uint8_t shtPinFrontP, uint8_t shtPinRightP);

  void init();

  //set Accuracy mode all
  void setAccuracyModeAll();

  //set Speed mode of all
  void setSpeedModeAll();

  //change ShtPin all
  void setShtAll(boolean state);

  void setShtSingle(boolean state, uint8_t sht);

  int getReading(int direction);

  void timeoutSensors();
};
