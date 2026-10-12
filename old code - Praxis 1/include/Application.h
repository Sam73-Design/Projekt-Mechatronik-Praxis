#pragma once

#include "ToFSensors.h"
#include "Drive.h"
#include "StackLogic.h"
#include "Led.h"

extern ToFSensors sensors;
extern Drive drive;
extern StackLogic logic;
extern Led led;

void changeMode();
void blackTile();
void linetrackerCalibration();
void startLoop();
void overHeating();
