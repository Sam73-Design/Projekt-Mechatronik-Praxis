#define LED1 2 //LED1 Pin
#define LED2 3 //LED2 Pin

long previousMillis1 = 0;  //Systemzeit beim letzten Umschalten von LED 1
long previousMillis2 = 0;  //Systemzeit beim letzten Umschalten von LED 2

long interval1 = 400;  //Umschaltintervall von LED 1 [ms]
long interval2 = 750;  //Umschaltintervall von LED 1 [ms]

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);      
}

void loop()
{

  //Aktuelle Systemzeit abfragen
  unsigned long currentMillis = millis();

  //Abfrage für LED 1
  if(currentMillis - previousMillis1 > interval1) {
    
    //Aktuelle Systemzeit speichern für das nächste Intervall
    previousMillis1 = currentMillis;   

    //LED umschalten
    digitalWrite(LED1, !digitalRead(LED1));
  }

  //Abfrage für LED 2
  if(currentMillis - previousMillis2 > interval2) {
    
    //Aktuelle Systemzeit speichern für das nächste Intervall
    previousMillis2 = currentMillis;   

    //LED umschalten
    digitalWrite(LED2, !digitalRead(LED2));
  }
}
