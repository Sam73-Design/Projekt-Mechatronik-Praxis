#define LED 3
#define BUTTON 2
#define DEBOUNCE_TIME 50 //Die Entprellzeit

//previousMillis - Zeitpunkt, an dem zuletzt der Button gedrückt wurde
unsigned long previousMillis = 0

boolean toogle_LED = false; //LED soll aktuell nicht umgeschaltet werden

//Wir definieren eine Liste an Zuständen, die wir states nennen.
//Am Ende muss ein Semikolon stehen
enum states { //Ein enum ist eine nummerierte Liste, 
              //Jedes Element hat eine Nummer, die jeweils höher ist 
              //als das vorhergehende Element
  buttonUnpressed, //0
  buttonPressed    //1
}; 

//Wir definieren eine Variable buttonState, die den aktuellen Zustand des 
//Tasters speichert
states buttonState = buttonUnpressed; 

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  //Interrupt Routine "interruptRoutine" soll ausgeführt werden,
  //wenn an Pin D2 (daher die 0 -> Interrupt-Vector für D2)
  //ein Interrupt ausgelöst wird
  attachInterrupt(0, interruptRoutine,CHANGE);
}

void loop() {
  //Hier kann jetzt sehr viel anderer Code hinein...
}

void interruptRoutine() { //Methode ist void und erhält keine Parameter
  
  switch (buttonState){ //Switch, der uns erlaubt die diskreten Zustände 
                        //abzuprüfen 
    
    case buttonUnpressed: //Der Button war vor Auslösen des Events nicht
                          //gedrückt
      if (!digitalRead(BUTTON)){//Ist der Button jetzt gedrückt?
        buttonState = buttonPressed; //In Zustand 1 wechseln
        previousMillis = millis(); //Aktuelle Systemzeit speichern
      } 
      break; //Ende dieses Cases

    case buttonPressed: //Der Button war vor Auslösen des Events gedrückt
      if (digitalRead(BUTTON)){//Wurde der Button losgelassen?
        buttonState = buttonUnpressed; //In Zustand 0 wechseln
        if (previousMillis - millis() > DEBOUNCE_TIME){ //Sind seit letztem 
                                                        //Event >50ms vergangen?
          toggleLED = true; //Button erfolgreich gedrückt -> LED umschalten
        }
      }
      break; //Ende dieses Cases
  } //Ende dieses Switches 

  //Prüfen, ob LED umgeschaltet werden soll
  if (toggleLED){
    toggleLED = false; //Reset
    digitalWrite(LED, !digitalRead(LED));
  }
  
}