#include <Arduino.h>
//Pins for L293D
#define E1 10  // Enable Pin für Motor 1
#define E2 11  // Enable Pin für Motor 2

#define C1 8   // Control Pin 1 für Motor 1
#define C2 9   // Control Pin 2 für Motor 1
#define C3 12  // Control Pin 1 für Motor 2
#define C4 13  // Control Pin 2 für Motor 2
 
void setup() {
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);
 
    pinMode(C1, OUTPUT);
    pinMode(C2, OUTPUT);
    pinMode(C3, OUTPUT);
    pinMode(C4, OUTPUT);
}
 
void loop() {
  
    //Beide Motoren in eine Richtung schalten
    digitalWrite(C1, HIGH);
    digitalWrite(C2, LOW);
    digitalWrite(C3, HIGH);
    digitalWrite(C4, LOW);

    for(int i = 0; i<255; i++){ //Drehzahl steigern
      analogWrite(E1,i);  
      analogWrite(E2,i);  
      delay(10);
    }
    for(int i = 255; i>0; i--){ //Drehzahl verringern
      analogWrite(E1,i); 
      analogWrite(E2,i); 
      delay(10);
    }
 
    // Motoren stoppen
    digitalWrite(E1, LOW);
    digitalWrite(E2, LOW);
    delay(200);
 
    //Beide Motoren in Gegenrichtung schalten
    digitalWrite(C1, LOW);
    digitalWrite(C2, HIGH);
    digitalWrite(C3, LOW);
    digitalWrite(C4, HIGH);

    for(int i = 0; i<255; i++){ //Drehzahl steigern
      analogWrite(E1,i);  
      analogWrite(E2,i);  
      delay(10);
    }
    for(int i = 255; i>0; i--){ //Drehzahl verringern
      analogWrite(E1,i); 
      analogWrite(E2,i); 
      delay(10);
    }

    // Motoren stoppen
    digitalWrite(E1, LOW);
    digitalWrite(E2, LOW);
    delay(200);
}
