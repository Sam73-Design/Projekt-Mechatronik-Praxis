//Code Beispiel für den Betrieb von zwei VL53L0X Sensoren
//Hier wird die Pololu Bibliothek genutzt
#include <Arduino.h>
#include <VL53L0X.h>
#include <Wire.h> //Die Wire library wird für die I2C Kommunikation benötigt

//Erstellen des VL53L0X Sensor Objekts
VL53L0X L0X1 = VL53L0X();

void setup() { 
  Serial.begin(9600);
  Wire.begin();
  
  //Sensor initialisieren
  if(!L0X1.init()){ //Sensor1 initialisieren und Antwort abwarten,
                    //wenn Antwort false, dann gab es einen Fehler
    Serial.println("Fehler in Sensor");
    while(1); //Arduino geht in Endlosschleife
  }

  //Sensor für kontinuierliche Messung im Modus HIGH ACCURACY einrichten 
  L0X1.setTimeout(500);
  L0X1.setMeasurementTimingBudget(200000); //Messung alle 200ms möglich
  L0X1.startContinuous();
  
  /*//Sensor für Einzelmessung im Modus HIGH SPEED einrichten
  L0X1.setTimeout(500);
  L0X1.setMeasurementTimingBudget(20000); //Messung alle 20ms möglich
  */
  
}

void loop() {

  //Sensor auslesen
  Serial.print("Sensor[mm]: ");
  Serial.println(L0X1.readRangeContinuousMillimeters());
  if (L0X1.timeoutOccurred()) { Serial.println(" TIMEOUT"); }
 
  delay(100);
}
