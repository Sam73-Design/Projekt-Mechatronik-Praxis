#define A_PIN 0 // analog pin connected to line tracker
#define D_PIN 5 // digital pin connected to line tracker

void setup() {
  pinMode(D_PIN,INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.print("Analog: ");
  Serial.print(analogRead(A_PIN));
  Serial.print("  Digital: ");
  Serial.println(digitalRead(D_PIN));
}
