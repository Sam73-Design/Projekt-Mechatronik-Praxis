#include <Servo.h> //Stellen Sie sicher, dass die Bibliothek Servo.h
                   //installiert ist.
Servo myServo;     //Neues Servo-Objekt erstellen

void setup() {
  myServo.attach(9);  //Den Servomotor an Pin D9 initialisieren
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 45){
      myServo.write(angle); //Servomotor auf definierten Winkel
                            //einstellen
      delay(500); //500ms warten, um dem Servomotor ausreichend
                  //Zeit zu geben, die neue Position anzufahren
    }
}
