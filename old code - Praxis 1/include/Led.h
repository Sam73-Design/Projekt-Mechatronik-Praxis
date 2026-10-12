#pragma once

#include <Arduino.h>

class Led {
  unsigned long prevMillis = 0;

public:
  void glow();

  void blink(int interval);

  void off();
};
