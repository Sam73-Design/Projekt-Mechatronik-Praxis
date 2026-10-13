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

//Erstellen der drei LED Objekte
LED led1(2,400);
LED led2(3,750);
LED led3(1,200);

void setup() {    
}

void loop()
{
  led1.abfragen();
  led2.abfragen();
  led3.abfragen();
}
