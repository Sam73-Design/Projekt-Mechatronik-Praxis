#include <Servo.h>

class LED //Die LED Klasse
{
  //Attribute
  int pin; //Der Pin, an den die LED angeschlossen ist
  long interval; //Der Ein/Aus-Intervall
  unsigned long previousMillis; //Die Systemzeit, bei der das letzte Umschalten
                                //stattgefunden hat

//Der Konstruktor
public:
LED(int pinP,long intervalP){
  //Zuweisen der Parameter auf die Attribute
  pin = pinP;
  interval = intervalP;
  pinMode(pin,OUTPUT);
  previousMillis = 0;
  }

//Die Methode zum Abfragen 
void abfragen()  {
    unsigned long currentMillis = millis();
     
    if(currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      digitalWrite(pin, !digitalRead(pin));  
    }   
  }
  
};

class Servomotor //Die Servomotor Klasse
{
  Servo servo; //ein Objekt der Servoklasse, über das der Servo
               //angesprochen wird
  int pin; //Pin, an den der Servo angeschlossen ist
  int pos; //Die aktuelle Servoposition
  int angleIncrement; //Winkel, um den der Servo bei jedem Schritt
                      //bewegt werden soll
  int interval; //Das Zeitintervall mit dem bewegt werden soll
  unsigned long previousMillis;

public:
Servomotor(int pinP, int intervalP, int angleIncrementP){
  pin = pinP;
  interval = intervalP;
  angleIncrement = angleIncrementP;
  }
  
void initialisieren(){ //Eine Methode, um den Servo zu initialisieren
                       //Muss in setup() ausgeführt werden
  servo.attach(pin);
}

void bewegen(){
    unsigned long currentMillis = millis();
     
    if(currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      pos += angleIncrement; //Die Servoposition um den gewünschten Winkel verschieben

      if (pos >= 180){ //Endwinkel in positive Richtung erreicht
        pos = 180;
        angleIncrement = -angleIncrement; //Drehrichtung umkehren
      }
      else if (pos <= 0){ //Endwinkel in negative Richtung erreicht
        pos = 0;
        angleIncrement = -angleIncrement; //Drehrichtung umkehren
      }
      servo.write(pos); //Servo auf neue Position setzen
    }
  }  
};

//Erstellen der drei LED Objekte
LED led1(2,400);
LED led2(3,750);
LED led3(1,200);

//Erstellen der zwei Servo Objekte
Servomotor s1(5,15,1);
Servomotor s2(6,350,15);

void setup() {    
s1.initialisieren();
s2.initialisieren();
}

void loop()
{
  led1.abfragen();
  led2.abfragen();
  led3.abfragen();

  s1.bewegen();
  s2.bewegen();
}
