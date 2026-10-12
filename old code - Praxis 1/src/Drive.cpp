#include "../include/Drive.h"
#include "../include/Config.h"

Drive::Drive()
    : motorRight(M2_F, M2_B, M2_EN),
      motorLeft(M1_F, M1_B, M1_EN),
      wallPID(PID_KP, PID_KI, PID_KD, PID_INTEGRAL_MAX) {
    qHead = 0;
    qTail = 0;
    startTime = 0;
    duration = 0;
    currentMotion = NONE;

    motorRight.init();
    motorLeft.init();
  }

void Drive::stop() {
    motorLeft.stop();
    motorRight.stop();
    delay(stopTime);
  }

void Drive::forward() {
    motorRight.forward();
    motorLeft.forward();
  }

void Drive::backward() {
    motorRight.backward();
    motorLeft.backward();
  }

void Drive::left() {
    motorRight.forward();
    motorLeft.backward();
  }

void Drive::right() {
    motorRight.backward();
    motorLeft.forward();
  }

void Drive::driveBack(int left, int right) {
    backward();
    motorLeft.power(left);
    motorRight.power(right);
    wallPID.reset();
  }

bool Drive::getIsMiddle() {
    return isMiddle;
  }

void Drive::setIsMiddle(bool state) {
    isMiddle = state;
  }

void Drive::goToMiddle(int dFront) {
    if (dFront < FRONT_T + 40 && dFront > FRONT_T - 10) {
      forward();
      motorLeft.power(BASE_SPEED * COMPENSATION_PWM);
      motorRight.power(BASE_SPEED);
    } else {

      stop();
      isMiddle = true;
    }
  }

void Drive::driveStraight(int difference) {
    forward();

    float correction = wallPID.compute(0, difference);

    // BASE_SPEED +- correction
    int leftPWM = BASE_SPEED * COMPENSATION_PWM + correction;
    int rightPWM = BASE_SPEED - correction;

    // constrain PWM signal
    leftPWM = constrain(leftPWM, MIN_PWM * COMPENSATION_PWM, MAX_PWM * COMPENSATION_PWM);
    rightPWM = constrain(rightPWM, MIN_PWM, MAX_PWM);

    motorLeft.power(leftPWM);
    motorRight.power(rightPWM);
  }

void Drive::driveAhead() {
    enqueue(AHEAD, driveTime);  //No alternativ to using delay yet
  }

void Drive::leftTurn() {
    enqueue(LEFT_TURN);
  }

void Drive::rightTurn() {
    enqueue(RIGHT_TURN);
  }

void Drive::deadEnd() {
    enqueue(DEAD_END);
  }

bool Drive::queueEmpty() {
    return qHead == qTail;
  }

bool Drive::queueFull() {
    return ((qTail + 1) % QUEUE_SIZE) == qHead;
  }

void Drive::clearQueue() {
    currentMotion = NONE;
    qHead = 0;
    qTail = 0;
  }

void Drive::enqueue(MotionType type, unsigned int duration) {
    if (queueFull()) return;
    queue[qTail] = { type, duration };
    qTail = (qTail + 1) % QUEUE_SIZE;
  }

void Drive::startMotion(Motion m) {
    delay(200);
    stop();

    wallPID.reset();

    switch (m.type) {

      case AHEAD:
        forward();
        motorLeft.power(BASE_SPEED * COMPENSATION_PWM + 10);
        motorRight.power(BASE_SPEED + 10);
        return;
        break;

      case LEFT_TURN:
        left();
        break;

      case RIGHT_TURN:
        right();
        break;

      case DEAD_END:
        left();
        break;
      default:
        break;
    }

    motorLeft.power(BASE_SPEED * COMPENSATION_PWM);
    motorRight.power(BASE_SPEED);

    startTime = millis();
    duration = m.duration;
    currentMotion = m.type;
  }

void Drive::update(byte mask) {
    if (currentMotion == NONE) {
      if (!queueEmpty()) {
        Motion m = queue[qHead];
        qHead = (qHead + 1) % QUEUE_SIZE;

        Serial.println("Start Motion");
        startMotion(m);
      }
      return;
    } else {
      checkDirection();  //if previously stuck, direction probably wrong
      switch (currentMotion) {
        case AHEAD:
          if (millis() - startTime > duration) {
            Serial.println("Stop Motion");
            currentMotion = NONE;
          }
          break;
        case LEFT_TURN:
          if (mask == (PATH_FRONT | PATH_LEFT)) {
            Serial.println("Stop Motion");
            stop();
            currentMotion = NONE;
          }
          break;
        case RIGHT_TURN:
          if ((mask & (PATH_RIGHT | PATH_FRONT)) == (PATH_RIGHT | PATH_FRONT) & ((((mask >> 3) ^ mask) & 1) ^ 1)) {
            Serial.println("Stop Motion");

            stop();
            currentMotion = NONE;
          }
          break;
        case DEAD_END:
          if (mask & PATH_FRONT) {
            Serial.println("Stop Motion");
            stop();
            currentMotion = NONE;
          }
          break;
      }
    }
  }

bool Drive::isBusy() {
    return currentMotion != NONE || !queueEmpty();
  }

void Drive::checkDirection() {
    switch (currentMotion) {
      case AHEAD:
        forward();
        break;
      case LEFT_TURN:
        left();
        break;
      case RIGHT_TURN:
        right();
        break;
      case DEAD_END:
        left();
        break;
    }
    motorLeft.power(BASE_SPEED * COMPENSATION_PWM);
    motorRight.power(BASE_SPEED);
  }
