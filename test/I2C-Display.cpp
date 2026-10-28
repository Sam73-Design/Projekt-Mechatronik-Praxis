#include <Arduino.h>
#include <string.h>
#include <I2CDisplay.h>

float temperatureC = 30.0f;
float motorSpeedRpm = 1250.0f;
int batteryPercent = 82;
bool systemActive = true;
char mode[5] = "Line";
int sensorValues[8] = {0,0,0,0,0,0,0,0};

void toggleMode(void *pointer) {
  char *text = static_cast<char *>(pointer);
  strcpy(text, strcmp(text, "Line") == 0 ? "Sort" : "Line");
}

DisplayVariable variables[] = {
    {"Temperature", &temperatureC, 'f'},
    {"Motor speed", &motorSpeedRpm, 'f'},
    {"Battery", &batteryPercent, 'i'},
    {"System active", &systemActive, 'b'},
    {"Mode", mode, 's', toggleMode},
    {"Linetrackers", sensorValues, 'l'},
};
I2CDisplay displayMenu(variables, sizeof(variables) / sizeof(variables[0]));

void setup() {
  Serial.begin(9600);
  if (!displayMenu.begin()) {
    Serial.println("OLED initialization failed");
  }
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

void loop() {
  temperatureC = 0.48828125f * analogRead(A0);
  sensorValues[0] = analogRead(A1);
  sensorValues[1] = analogRead(A2);
  sensorValues[2] = analogRead(A3);
  sensorValues[3] = sensorValues[0] * 2;
  sensorValues[4] = sensorValues[1] * 2;
  sensorValues[5] = sensorValues[2] * 2;
  sensorValues[6] = sensorValues[0] * 3;
  sensorValues[7] = sensorValues[1] * 3;
  displayMenu.update();
}
