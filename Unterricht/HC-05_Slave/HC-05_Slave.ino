#include <SoftwareSerial.h>
#define LED 3

#define BTRX 4    //-> an den TX Pin des HC-05 anschließen
#define BTTX 5 //-> an den RX Pin des HC-05 anschließen
SoftwareSerial SerialBT(BTRX, BTTX);


void setup() {  
  Serial.begin(9600);    // Serieller Monitor
  SerialBT.begin(9600);  // Bluetooth / HC-05
  digitalWrite(LED,LOW);
  pinMode(LED,OUTPUT);
}

void loop() {  
  char c;
  // Liegen Daten am BLE-Modul an?
  if (SerialBT.available()) {
    c = SerialBT.read(); //Daten auslesen und in Variable c speichern
    switch(c){
      case 'b': //LED ausschalten
      digitalWrite(LED,LOW);
      break;
      
      case 'B': //LED einschalten
      digitalWrite(LED,HIGH);
      break;
      } 
  }
}
