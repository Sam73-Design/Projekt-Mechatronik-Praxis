/*
Sunfounder line follower module 
returns 2 bytes per sensor
following code reads all sensors and stores values as uint16_t*/

#include <Arduino.h>
#include <Wire.h>

#define SENSOR_ADDRESS 0x09
#define NUM_SENSORS 8
#define DATA_BYTES 16

byte sensorData[DATA_BYTES];

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Standard I2C speed
  Wire.setClock(100000);

  Serial.println("SunFounder 8-Channel Line Follower");
  Serial.println("Starting sensor...");
  delay(500);
}

void loop() {
  // Request 16 bytes from the sensor
  byte received = Wire.requestFrom(
    (uint8_t)SENSOR_ADDRESS,
    (uint8_t)DATA_BYTES
  );

  if (received == DATA_BYTES) {

    // Read all 16 bytes
    for (int i = 0; i < DATA_BYTES; i++) {
      sensorData[i] = Wire.read();
    }

    // Display the two bytes for each channel
    Serial.println("Sensor readings:");

    for (int i = 0; i < NUM_SENSORS; i++) {
      int index = i * 2;

      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.print(": ");
      //shift first 8 bits to left and add second 8 bits to form a 16 bit value
      uint16_t value = (sensorData[index] << 8) | sensorData[index + 1];
      Serial.println(value);
    }

    Serial.println("------------------------");
  }
  else {
    // Clear any remaining bytes
    while (Wire.available()) {
      Wire.read();
    }

    Serial.print("I2C read failed. Bytes: ");
    Serial.println(received);
  }

  delay(500);
}
