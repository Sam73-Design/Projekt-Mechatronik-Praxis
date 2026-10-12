#pragma once

#include <Arduino.h>

extern int lastFront;
extern byte lastFrontBit;
extern int stuckAttempts;
extern int lineTrackerVal;

// tracking time
extern unsigned long previousMillis;
extern unsigned long stuckCheckTime;
extern unsigned long overHeatingTime;
extern unsigned long crossingTime;

extern bool abortRun;     //Error occured --> stop motors
extern volatile bool stp;  //setup
extern volatile bool buttonPressed;
extern volatile bool calibrated;   //calibrate linetracker
extern volatile bool onBlackTile;  //reached finish
extern volatile bool solveMode;
extern volatile bool restartRun;  //restart speedrun
