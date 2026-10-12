#include <Arduino.h>

const int ledPin = LED_BUILTIN;

void setup() {
	pinMode(ledPin, OUTPUT);
}

//Function must be declared before it is used in the loop function
void ledBlink(int times, int duration) {
    for (int i = 0; i < times; i++) {
        digitalWrite(ledPin, HIGH);
        delay(duration);
        digitalWrite(ledPin, LOW);
        delay(duration);
    }
}

void loop() {
    ledBlink(3,500);
    ledBlink(5,200);
    ledBlink(10,100);
}


