#include <Arduino.h>
#define TRIG 7
#define ECHO 6

int distanz;
void ping(int trigPin);
int measure(int echoPin);

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT); 
  Serial.begin(9600); //Serielle Kommunikation initialisieren mit
                      //9600 Baud
  digitalWrite(TRIG,LOW); //Trigger auf LOW initialisieren
  delayMicroseconds(2);
}
void loop() {
  ping(TRIG); 
  distanz = measure(ECHO);

  //Bildschirmausgabe
  Serial.print("Abstand: ");
  Serial.print(distanz);
  Serial.println("mm");

  delay(500);
}

void ping(int trigPin){
  //Ultraschallimpuls auslösen
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

int measure(int echoPin){
  //Dauer des HIGH-Signals am ECHO Pin messen
  long zeit = pulseIn(echoPin, HIGH);
  //Abstand ausrechnen und zurückgeben
  return zeit*0.0343/2*10; //in cm
}
