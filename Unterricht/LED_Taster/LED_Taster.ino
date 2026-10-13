#define LED 3
#define BUTTON 2

void setup() {
  pinMode(LED,OUTPUT);
  pinMode(BUTTON,INPUT); //Pin D2 wird als Eingang definiert

}

void loop() {
  int buttonState = digitalRead(BUTTON); //Den Zustand des Tasters
                                         //an D2 auslesen
  digitalWrite(LED, buttonState); //LED entsprechend schalten
}
