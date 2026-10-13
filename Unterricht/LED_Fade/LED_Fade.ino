#define LED 3 

void setup() {
 pinMode(LED,OUTPUT); 
}

void loop() {
  //LED langsam heller werden lassen
  //Schleife, die von 0 bis 254 in 1er Schritten läuft
  for (int i = 0; i<255; i++){
    analogWrite(LED, i); //aktuelle Spannung ist 5V*i/255
    delay(20); //20ms warten
  }

  //LED langsame dunkler werden lassen
  //Schleife, die von 255 bis 1 in 1er Schritten läuft
  for (int i = 255; i>0; i--){
    analogWrite(LED, i); 
    delay(20); 
  }
  
}
