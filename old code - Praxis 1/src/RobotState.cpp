#include "../include/RobotState.h"

int lastFront = 0;
byte lastFrontBit = 0b0000;
int stuckAttempts = 0;
int lineTrackerVal = 0;

// tracking time
unsigned long previousMillis = 0;
unsigned long stuckCheckTime = 0;
unsigned long overHeatingTime = 0;
unsigned long crossingTime = 0;

bool abortRun = false;     //Error occured --> stop motors
volatile bool stp = true;  //setup
volatile bool buttonPressed = false;
volatile bool calibrated = false;   //calibrate linetracker
volatile bool onBlackTile = false;  //reached finish
volatile bool solveMode = false;
volatile bool restartRun = false;  //restart speedrun
