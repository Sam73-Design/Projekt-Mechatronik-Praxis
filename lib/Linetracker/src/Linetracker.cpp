#include "Linetracker.h"

Linetracker::Linetracker(TwoWire &bus, uint8_t sensorAddress)
    : wire(bus), address(sensorAddress) {}

void Linetracker::begin() {
  wire.begin();
  wire.setClock(100000);
  lastReadSucceeded = false;
  receivedBytes = 0;
}

bool Linetracker::read() {
  receivedBytes = wire.requestFrom(address, DATA_BYTES);
  if (receivedBytes != DATA_BYTES || wire.available() < DATA_BYTES) {
    while (wire.available()) wire.read();
    lastReadSucceeded = false;
    return false;
  }

  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {
    const uint16_t highByte = static_cast<uint8_t>(wire.read());
    const uint16_t lowByte = static_cast<uint8_t>(wire.read());
    sensorValues[i] = (highByte << 8) | lowByte;
  }
  lastReadSucceeded = true;
  return true;
}
