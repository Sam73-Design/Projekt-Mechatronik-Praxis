
#include <MFRC522.h> //MFRC522 Bibliothek fuer Kommunikation mit RFID Reader einbinden
#include <SPI.h> //SPI Bibliothek fuer Nutzung des SPI-Protokolls einbinden

#define SSPIN 10 //SS Pin fuer SPI Kommunikation
#define RESETPIN 9 //RST Pin fuer SPI Kommunikation

byte read_UID[4] = {0,0,0,0}; //Variable in der die gelesene UID gespeichert wird

MFRC522 RFIDreader(SSPIN,RESETPIN);

unsigned long old_time = 0;

#define WAIT_TIME 1000 //1000ms Wartezeit bis neue UID gelesen wird

void setup() {
  SPI.begin(); //SPI Bus starten
  Serial.begin(9600); //Serielle Kommunikation starten
  RFIDreader.PCD_Init(); //RFID Leser initialisieren
  old_time = millis(); //aktuelle Zeit speichern
  Serial.println("Start");
}

void loop() {
  if (!RFIDreader.PICC_IsNewCardPresent()) { //Wenn keine Karte in der Nähe ist,
                                             //zurück zum Beginn von void loop()
    return;
  } 
    
  if (!RFIDreader.PICC_ReadCardSerial()) { //Wenn die Karte noch nicht gelesen wurde,
                                           //zurück zum Beginn von void loop()
    return;
  }

  //Ab hier wird ausgeführt wenn Karte in der Nähe und gelesen
  
  //Nur ausführen, wenn eine neue Karte detektiert wurde bzw.
  //wenn die aktuelle Karte dazwischen entfernt wurde
  if((millis() - old_time)>WAIT_TIME){

      //UID lesen
      for (int i = 0; i < 4; i++){
        read_UID[i] = RFIDreader.uid.uidByte[i];
      }
      Serial.print("Gelesene UID: ");
      for (int i = 0; i < 4; i++){
        Serial.print(read_UID[i],HEX); //UID Blöcke einzeln in Hex ausgeben
        Serial.print(" "); //Dazwischen Leerzeichen zur besseren Lesbarkeit
      }
      Serial.println();
  }
    
  old_time = millis(); //aktuelle Zeit speichern
}
