#include <Arduino.h>
#include <Linetracker.h>

Linetracker linetracker;

void setup() {
  Serial.begin(9600);
  linetracker.begin();
  Serial.println("SunFounder 8-Channel Line Follower");
  Serial.println("Starting sensor...");
  delay(500);
}

void loop() {
  if (linetracker.read()) {
    Serial.println("Sensor readings:");
    for (uint8_t i = 0; i < Linetracker::SENSOR_COUNT; ++i) {
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(linetracker.values()[i]);
    }
    Serial.println("------------------------");
  } else {
    Serial.print("I2C read failed. Bytes: ");
    Serial.println(linetracker.bytesReceived());
  }
  delay(500);
}
