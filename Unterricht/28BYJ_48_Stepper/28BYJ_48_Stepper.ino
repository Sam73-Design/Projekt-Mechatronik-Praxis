#include <Stepper.h>

Stepper myStepper(32, 10, 12, 11, 13);
int steps = 2038; //Volle Drehung im Uhrzeigersinn

void setup() {
  myStepper.setSpeed(1000); //Schrittgeschwindigkeit setzen
}

void loop() {
  myStepper.step(steps); //Bestimmte Anzahl Schritte ausführen
  steps = steps*-1; //Drehrichtung umkehren
}
