#pragma once
#include <Arduino.h>
#include <Wire.h>

// SunFounder eight-channel module: two bytes per sensor, high byte first.
class Linetracker {
public:
  static constexpr uint8_t SENSOR_COUNT = 8;
  static constexpr uint8_t DATA_BYTES = SENSOR_COUNT * 2;
  explicit Linetracker(TwoWire &wire = Wire, uint8_t address = 0x09);
  void begin(); // Initializes the bus at 100 kHz; does not probe the module.
  bool read(); // False leaves the last readings intact.
  const uint16_t *values() const { return sensorValues; } // Indices 0..7.
  bool connected() const { return lastReadSucceeded; }
  uint8_t bytesReceived() const { return receivedBytes; }

private:
  TwoWire &wire;
  uint8_t address;
  uint16_t sensorValues[SENSOR_COUNT] = {};
  bool lastReadSucceeded = false;
  uint8_t receivedBytes = 0;
};
