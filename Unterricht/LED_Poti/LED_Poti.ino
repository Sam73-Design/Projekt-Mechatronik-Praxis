#define LED 3
#define POTI 0 //Analoger Pin, an dem das Potentiometer angeschlossen ist

void setup() {
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH); //LED anschalten
}

void loop() {
  int potiVal = analogRead(POTI);
  
  //Den an A0 ausgelesenen Wert (0-1023) auf 20-1000 umrechnen
  int delayVal = map(potiVal,0,1023,20,1000); 
  digitalWrite(LED, !digitalRead(LED)); //LED umschalten
  delay(delayVal); //entsprechend der Poti-Stellung warten
}
