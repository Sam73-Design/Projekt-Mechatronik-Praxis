#pragma once

#include <Arduino.h>
#include "Motor.h"
#include "PIDController.h"

class Drive {
private:
  const int driveTime = 1300;
  const int stopTime = 300;  //waiting until full stop
  bool isMiddle = false;

  //Pin layout: (Forward, Backward, Enable)
  Motor motorRight;  //create instances
  Motor motorLeft;
  PIDController wallPID;


  enum MotionType {
    LEFT_TURN,  //0...
    AHEAD,
    RIGHT_TURN,
    DEAD_END,
    NONE  //...4
  };

  struct Motion {
    MotionType type;
    unsigned int duration;
  };

  static const int QUEUE_SIZE = 4;
  Motion queue[QUEUE_SIZE];
  int qHead;
  int qTail;

  //information current motion
  MotionType currentMotion;
  unsigned long startTime;
  unsigned int duration;


public:
  Drive();

  void stop();

  void forward();

  void backward();

  void left();

  void right();

  void driveBack(int left, int right);

  bool getIsMiddle();

  void setIsMiddle(bool state);

  void goToMiddle(int dFront);

  //using PID controller
  void driveStraight(int difference);

  void driveAhead();

  void leftTurn();

  void rightTurn();

  //dead end
  void deadEnd();

  bool queueEmpty();

  bool queueFull();

  void clearQueue();

  // put the next motion in the array
  void enqueue(MotionType type, unsigned int duration = 0);

  void startMotion(Motion m);

  // update motion
  void update(byte mask);

  bool isBusy();

  void checkDirection();
};
