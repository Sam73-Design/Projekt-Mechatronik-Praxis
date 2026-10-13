//Code Beispiel für den Betrieb von zwei VL53L0X Sensoren
//Hier wird die Pololu Bibliothek genutzt
#include <Arduino.h>
#include <VL53L0X.h>
#include <Wire.h> //Die Wire library wird für die I2C Kommunikation benötigt

//Die zu belegenden I2C Adressen der Sensoren
#define VL53L0X1_ADDRESS 01
#define VL53L0X2_ADDRESS 02

//Die belegten Shutdown Pins zum einzelnen Ansteuern der Sensoren
#define SHT_1 2
#define SHT_2 4

//Erstellen der beiden VL53L0X Sensor Objekte
VL53L0X L0X1 = VL53L0X();
VL53L0X L0X2 = VL53L0X();



void setup() {
  //Shutdown Pins als Output festlegen
  pinMode(SHT_1,OUTPUT);
  pinMode(SHT_2,OUTPUT);
  
  Serial.begin(9600);
  Wire.begin();
  
  //Belegen der neuen Adressen
  //Shutdown Pins aller Sensoren aktivieren -> alle Sensoren werden abgeschaltet
  digitalWrite(SHT_1,LOW);
  digitalWrite(SHT_2,LOW);
  delay(10);

  //Sensor 1 aktivieren und neue Adresse zuweisen
  digitalWrite(SHT_1,HIGH);
  delay(10);
  L0X1.setAddress((uint8_t)VL53L0X1_ADDRESS);

  //Sensor 2 aktivieren und neue Adresse zuweisen
  digitalWrite(SHT_2,HIGH);
  delay(10);
  L0X2.setAddress((uint8_t)VL53L0X2_ADDRESS);

  //Sensoren initialisieren
  if(!L0X1.init()){ //Sensor1 initialisieren und Antwort abwarten,
                    //wenn Antwort false, dann gab es einen Fehler
    Serial.println("Fehler in Sensor1");
    while(1); //Arduino geht in Endlosschleife
  }
  if(!L0X2.init()){ //Sensor2 initialisieren und Antwort abwarten,
                    //wenn Antwort false, dann gab es einen Fehler
    Serial.println("Fehler in Sensor2");
    while(1); //Arduino geht in Endlosschleife
  }

  //Sensor 1 für kontinuierliche Messung im Modus HIGH ACCURACY einrichten 
  L0X1.setTimeout(500);
  L0X1.setMeasurementTimingBudget(200000); //Messung alle 200ms möglich
  L0X1.startContinuous();
  
  //Sensor2 für Einzelmessung im Modus HIGH SPEED einrichten
  L0X2.setTimeout(500);
  L0X2.setMeasurementTimingBudget(20000); //Messung alle 20ms möglich
  
  
}

void loop() {

  //Sensor1 auslesen
  Serial.print("Sensor1[mm]: ");
  Serial.print(L0X1.readRangeContinuousMillimeters());
  if (L0X2.timeoutOccurred()) { Serial.println(" TIMEOUT"); }

  //Sensor2 auslesen
  Serial.print("  Sensor2[mm]: ");
  Serial.println(L0X2.readRangeSingleMillimeters());
  if (L0X2.timeoutOccurred()) { Serial.println(" TIMEOUT"); }
 
  delay(100);
}
