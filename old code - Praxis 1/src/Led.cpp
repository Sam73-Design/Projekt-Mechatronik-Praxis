#include "../include/Led.h"
#include "../include/Config.h"

void Led::glow() {
    digitalWrite(LED, HIGH);
  }

void Led::blink(int interval) {
    if (millis() - prevMillis > interval) {
      digitalWrite(LED, !digitalRead(LED));
      prevMillis = millis();
    }
  }

void Led::off() {
    digitalWrite(LED, LOW);
  }
