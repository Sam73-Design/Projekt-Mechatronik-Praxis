#include <Arduino.h>
#include <VL53L0X.h>
#include <Wire.h>
#include <Stack.h>

/*

A4 connected with SDA-pin of all ToF VL53L0X
A5 connected with SCL-pin of all ToF VL53L0X

VCC +5 for all sensors and the motor driver
GND of Nano connected with GND of battery
Vin connected with VCC +12 of battery
*/

#define BTN_PIN 2       //Button to change mode
#define LINETRACKER A0  //A0 of Linetracker
#define M1_B 6          //Backward pin motor 1
#define M1_F 7          //Forward pin motor 1
#define M1_EN 5         //Enable pin motor 1
#define M2_F 8          //Forward pin Motor 2
#define M2_B 4          //Backward pin motor 2
#define M2_EN 9         //Enable pin motor 2
#define SHT_L 11        //SHT pin ToF left
#define SHT_F 10        //SHT pin ToF front
#define SHT_R 12        //SHT pin ToF right
#define LED 13          //LED Pin

#define DEBOUNCE_TIME 50  //Entprellzeit

#define MIN_PWM 45  //Minimum Power for the motors
#define MAX_PWM 90  //Maximum Power for the motors
#define BASE_SPEED 55
#define COMPENSATION_PWM 1.214  //compensate left motor; left spins slower

//All values should be calculated based on the dimensions of the bot and position of the sensors
#define FRONT_T 65     //distance at which path ends.
#define SIDE_T 100     //distance at which path is existend
#define L0X_COMP_L 48  //calibration of sensors
#define L0X_COMP_R 7

#define TRACKER_OFFSET 100  //Linetracker offset

// PID-Parameter (zum einfachen Tuning)
#define PID_KP 0.2             // Range: 0.1 - 0.6
#define PID_KI 0.0015          // Range: 0.001 - 0.005
#define PID_KD 0.015           // Range: 0.01 - 0.03
#define PID_INTEGRAL_MAX 20.0  // Anti-Windup Limit;

// Steckenbleiben-Erkennung
#define STUCK_DETECTION_THRESHOLD 1  // Change < x = stuck
#define STUCK_INTERVALL 2000
#define MAX_STUCK_ATTEMPTS 20  // Max. tries

// used for bit-operation
#define LF 0b1000  //last front open
#define R 0b0100   //right open
#define F 0b0010   //front open
#define L 0b0001   //left open

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

//---ToFSensors---
class ToFSensors {

  uint8_t shtPin[3];
  uint8_t address[3] = { 0x30, 0x31, 0x32 };
  VL53L0X L0X[3] = { VL53L0X(), VL53L0X(), VL53L0X() };  //left, front, right

public:
  ToFSensors(uint8_t shtPinLeftP, uint8_t shtPinFrontP, uint8_t shtPinRightP) {
    shtPin[0] = shtPinLeftP;
    shtPin[1] = shtPinFrontP;
    shtPin[2] = shtPinRightP;
  }

  void init() {
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

  //set Accuracy mode all
  void setAccuracyModeAll() {
    for (int i = 0; i < 3; i++) {
      L0X[i].setMeasurementTimingBudget(200000);
    }
  }

  //set Speed mode of all
  void setSpeedModeAll() {
    for (int i = 0; i < 3; i++) {
      L0X[i].setMeasurementTimingBudget(20000);
    }
  }

  //change ShtPin all
  void setShtAll(boolean state) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(shtPin[i], state);
    }
  }

  void setShtSingle(boolean state, uint8_t sht) {
    digitalWrite(shtPin[sht], state);
  }

  int getReading(int direction) {
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

  void timeoutSensors() {
    for (int i = 0; i < 3; i++) {
      if (L0X[i].timeoutOccurred()) {
        Serial.print("Timeout sensor ");
        Serial.println(i);
      }
    }
  }
};

//---Motor---
class Motor {
private:
  int pinV;   //Forward pin
  int pinB;   //Backward pin
  int pinEn;  //Enable Pin; PWM

public:
  Motor(int pinVP, int pinBP, int pinEnP) {
    pinV = pinVP;
    pinB = pinBP;
    pinEn = pinEnP;
  }

  void init() {
    pinMode(pinV, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinEn, OUTPUT);
  }

  void forward() {
    digitalWrite(pinV, HIGH);
    digitalWrite(pinB, LOW);
  }

  void backward() {
    digitalWrite(pinV, LOW);
    digitalWrite(pinB, HIGH);
  }

  void power(int value) {
    analogWrite(pinEn, value);
  }

  void stop() {
    digitalWrite(pinV, LOW);
    digitalWrite(pinB, LOW);
    power(0);
  }
};

//---PIDController---
class PIDController {
private:
  float kp;  // Proportional gain
  float ki;  // Integral gain
  float kd;  // Derivative gain

  float lastError;
  float integral;
  float integralMax;  // Anti-Windup limit

  unsigned long lastTime;

public:
  PIDController(float p, float i, float d, float intMax = 100.0)
    : kp(p), ki(i), kd(d), integralMax(intMax), lastError(0), integral(0), lastTime(0) {}

  // compute value based on error
  float compute(float setpoint, float measured) {
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;

    // first call or long pause
    if (lastTime == 0 || dt > 1.0) {
      lastTime = now;
      lastError = 0;
      integral = 0;
      return 0;
    }
    lastTime = now;

    float error = setpoint - measured;

    // Integral (with Anti-Windup)
    integral += error * dt;
    integral = constrain(integral, -integralMax, integralMax);

    // Derivative
    float derivative = (error - lastError) / dt;
    lastError = error;

    // PID-formula
    float output = kp * error + ki * integral + kd * derivative;
    return output;
  }

  //reset controller
  void reset() {
    lastError = 0;
    integral = 0;
    lastTime = 0;
  }

  void printGains() {
    Serial.print(" \ LastError: ");
    Serial.print(lastError);
    Serial.print(" \ Integral: ");
    Serial.println(integral);
  }
};


//---Drive---
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
  Drive()
    : wallPID(PID_KP, PID_KI, PID_KD, PID_INTEGRAL_MAX),
      motorLeft(M1_F, M1_B, M1_EN),
      motorRight(M2_F, M2_B, M2_EN) {
    qHead = 0;
    qTail = 0;
    startTime = 0;
    duration = 0;
    currentMotion = NONE;

    motorRight.init();
    motorLeft.init();
  }

  void stop() {
    motorLeft.stop();
    motorRight.stop();
    delay(stopTime);
  }

  void forward() {
    motorRight.forward();
    motorLeft.forward();
  }

  void backward() {
    motorRight.backward();
    motorLeft.backward();
  }

  void left() {
    motorRight.forward();
    motorLeft.backward();
  }

  void right() {
    motorRight.backward();
    motorLeft.forward();
  }

  void driveBack(int left, int right) {
    backward();
    motorLeft.power(left);
    motorRight.power(right);
    wallPID.reset();
  }

  bool getIsMiddle() {
    return isMiddle;
  }

  void setIsMiddle(bool state) {
    isMiddle = state;
  }

  void goToMiddle(int dFront) {
    if (dFront < FRONT_T + 40 && dFront > FRONT_T - 10) {
      forward();
      motorLeft.power(BASE_SPEED * COMPENSATION_PWM);
      motorRight.power(BASE_SPEED);
    } else {

      stop();
      isMiddle = true;
    }
  }

  //using PID controller
  void driveStraight(int difference) {
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

  void driveAhead() {
    enqueue(AHEAD, driveTime);  //No alternativ to using delay yet
  }

  void leftTurn() {
    enqueue(LEFT_TURN);
  }

  void rightTurn() {
    enqueue(RIGHT_TURN);
  }

  //dead end
  void deadEnd() {
    enqueue(DEAD_END);
  }

  bool queueEmpty() {
    return qHead == qTail;
  }

  bool queueFull() {
    return ((qTail + 1) % QUEUE_SIZE) == qHead;
  }

  void clearQueue() {
    currentMotion = NONE;
    qHead = 0;
    qTail = 0;
  }

  // put the next motion in the array
  void enqueue(MotionType type, unsigned int duration = 0) {
    if (queueFull()) return;
    queue[qTail] = { type, duration };
    qTail = (qTail + 1) % QUEUE_SIZE;
  }

  void startMotion(Motion m) {
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

  // update motion
  void update(byte mask) {
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
          if (mask == (F | L)) {
            Serial.println("Stop Motion");
            stop();
            currentMotion = NONE;
          }
          break;
        case RIGHT_TURN:
          if ((mask & (R | F)) == (R | F) & ((((mask >> 3) ^ mask) & 1) ^ 1)) {
            Serial.println("Stop Motion");

            stop();
            currentMotion = NONE;
          }
          break;
        case DEAD_END:
          if (mask & F) {
            Serial.println("Stop Motion");
            stop();
            currentMotion = NONE;
          }
          break;
      }
    }
  }

  bool isBusy() {
    return currentMotion != NONE || !queueEmpty();
  }

  void checkDirection() {
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
};

//---StackLogic---
class StackLogic {
  // left-turn = 0, straight on intersection = 1, right-turn = 2
  Stack<int> path;
  Stack<int> solution;
  Stack<int> backUpSolution;  //used when restarting after an error during solving
  int lastPath = -1;          //re-use lastPath as nextDirection for solution

public:
  //the logic simplifies the path and removes dead ends and associated turns
  void rightTurnLogic() {
    switch (lastPath) {
      case (-2):
        abortRun = true;
        break;
      case (-1):
        path.push(2);
        break;
      case 0:
        path.pop();
        peekLastDirectionPath();
        break;
      case 1:
        path.pop();
        path.push(0);
        lastPath = -1;
        break;
      case 2:
        path.pop();
        path.push(1);
        lastPath = -1;
        break;
    }
  }

  void leftTurnLogic() {
    switch (lastPath) {
      case (-2):
        abortRun = true;
        break;
      case (-1):
        path.push(0);
        break;
        /*
          not possible or already covered
        case 0:
          break;
          
        case 1:
          break;
          */
      case 2:
        path.pop();
        peekLastDirectionPath();
        break;
    }
  }

  void driveAheadLogic() {
    switch (lastPath) {
      case (-2):
        abortRun = true;
        break;
      case (-1):
        path.push(1);
        break;
        /*
          not possible or already covered
        case 0:
          break;
          */
      case 1:
        path.pop();
        peekLastDirectionPath();
        break;
      case 2:
        path.pop();
        path.push(0);
        lastPath = -1;
        break;
    }
  }

  void peekLastDirectionPath() {
    lastPath = (path.length() >= 1) ? path.peek() : -2;
  }

  void popLastDirectionSolution() {
    lastPath = (solution.length() >= 1) ? solution.pop() : -2;
  }

  boolean checkPathLength() {
    return (path.length() >= 1);
  }

  boolean checkSolutionLength() {
    return (solution.length() >= 1);
  }

  int getLastPath() {
    return lastPath;
  }

  void savePath() {
    if (path.length() < solution.length() || solution.length() == 0) {
      Stack<int> temp;
      temp.concat(path);
      while (temp.length() > 0) {
        // reverse Stack
        int tmp = temp.pop();
        solution.push(tmp);
        backUpSolution.push(tmp);
      }
    }
  }

  void restoreSolution() {
    solution.clear();
    solution.concat(backUpSolution);
  }
};

//---Led---
class Led {
  unsigned long prevMillis = 0;

public:
  void glow() {
    digitalWrite(LED, HIGH);
  }

  void blink(int interval) {
    if (millis() - prevMillis > interval) {
      digitalWrite(LED, !digitalRead(LED));
      prevMillis = millis();
    }
  }

  void off() {
    digitalWrite(LED, LOW);
  }
};

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

  if ((mask & (R | F | L)) == F) {  //only front open
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

    if (mask & R) {  //right open
      Serial.println("Turn right ");
      logic.rightTurnLogic();
      drive.rightTurn();
      drive.driveAhead();

    } else if (mask & F) {  //front open
      Serial.println("Go ahead ");
      logic.driveAheadLogic();
      drive.driveAhead();

    } else if (mask & L) {  //left open
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
    crossingTime = millis;
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
        if (mask & L) {
          drive.leftTurn();
        }
        break;
      //go straight
      case 1:
        if (mask & F) {
          //drive.driveAhead(); at the end
        }
        break;
      //go right
      case 2:
        if (mask & R) {
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
