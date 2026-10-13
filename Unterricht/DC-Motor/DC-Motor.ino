#define DCMOTOR 6 //Pin, der an der Basis des Transistors 
                  //angeschlossen ist
 
void setup() 
{ 
  pinMode(DCMOTOR, OUTPUT);
} 
 
 
void loop() 
{ 
  for(int i = 0; i<255; i++){ //Drehzahl steigern
    analogWrite(DCMOTOR,i);  
    delay(50);
  }
  for(int i = 255; i>0; i--){ //Drehzahl verringern
    analogWrite(DCMOTOR,i); 
    delay(10);
  }
} 
