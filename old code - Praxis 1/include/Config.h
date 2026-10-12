#pragma once

#include <Arduino.h>

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
#define PATH_LAST_FRONT 0b1000  //last front open
#define PATH_RIGHT 0b0100   //right open
#define PATH_FRONT 0b0010   //front open
#define PATH_LEFT 0b0001   //left open
