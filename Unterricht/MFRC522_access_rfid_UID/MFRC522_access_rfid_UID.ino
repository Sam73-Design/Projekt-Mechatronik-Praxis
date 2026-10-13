
#include <MFRC522.h> //MFRC522 Bibliothek fuer Kommunikation mit RFID Reader einbinden
#include <SPI.h> //SPI Bibliothek fuer Nutzung des SPI-Protokolls einbinden

#define SSPIN 10 //SS Pin fuer SPI Kommunikation
#define RESETPIN 9 //RST Pin fuer SPI Kommunikation
#define LEDGREEN 8 //Green LED Pin
#define LEDRED 7 //Red LED Pin

byte valid_UID[4] = {0xA6,0x73,0x53,0x83}; //gültige UID in HEX

MFRC522 RFIDreader(SSPIN,RESETPIN);

//Wire.beginTransmission()

unsigned long old_time = 0;
boolean card_removed = true;

#define WAIT_TIME 500 //500ms Wartezeit bis neue UID gelesen wird

void setup() {
  SPI.begin(); //SPI Bus starten
  Serial.begin(9600); //Serielle Kommunikation starten
  RFIDreader.PCD_Init(); //RFID Leser initialisieren
  pinMode(LEDGREEN,OUTPUT);
  pinMode(LEDRED,OUTPUT);
}

void loop() {
  if (!RFIDreader.PICC_IsNewCardPresent()) { //Wenn keine Karte in der Nähe ist,
                                             //zurück zum Beginn von void loop()
                                             
     if((millis() - old_time)>WAIT_TIME){ //Karte wurde entfernt
        
        if (!card_removed){ //Pruefen ob Karte schon als "removed" markiert wurde
          Serial.println("Card removed");
          digitalWrite(LEDGREEN,LOW);
          digitalWrite(LEDRED,LOW);
          card_removed = true;}
      }
      
    return;
    } 
    
  if (!RFIDreader.PICC_ReadCardSerial()) { //Wenn die Karte noch nicht gelesen wurde,
                                           //zurück zum Beginn von void loop()
    return;
    }

  //Ab hier wird ausgeführt wenn Karte in der Nähe und gelesen
  
    //UID pruefen
    if(compareByteArray(valid_UID,RFIDreader.uid.uidByte)){
          
      //Ab hier wenn UID gueltig ist      
        if (card_removed){ //Wenn Karte zuvor entfernt worden war
          Serial.println("Access");
          digitalWrite(LEDGREEN,HIGH);
          card_removed = false;
          }
    
    }
    
    else{ //Wenn UID ungueltig
      if (card_removed){ //Wenn Karte zuvor entfernt worden war
        Serial.println("Access denied");
        digitalWrite(LEDRED,HIGH);
        card_removed = false;
        }
    }

    old_time = millis(); //old_time auf die aktuelle Zeit setzen
}

//Funktion zum Vergleich der beiden Byte Arrays
boolean compareByteArray (byte byteArray1[], byte byteArray2[]){

  for (int i = 0; i < 4; i++){ //Die UID byte arrays haben immer 4 Stellen
    if (byteArray1[i]!=byteArray2[i]){return false;} //Stimmt eine der Stellen nicht
                                                     //überein, wird abgebrochen und 
                                                     //false zurückgegeben
    }

  return true;
  }
