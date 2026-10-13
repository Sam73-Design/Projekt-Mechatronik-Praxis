#define S_PIN 4 // digital pin connected to S pin on the light barrier

void setup() {
  pinMode(S_PIN,INPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(S_PIN)){ // Object in light path
    Serial.println("Detection");
    }
  else{Serial.println("No detection");}
}
