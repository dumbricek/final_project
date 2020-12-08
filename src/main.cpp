
#include <Arduino.h>
const int lP = 2;
const int pP = 3;
const int vP = 4;
boolean pS = false;
boolean lS = false;
boolean vS = false;
// inicializace proměnných, do kterých se uloží data
void podminka(char s){
  Serial.println(s);
  if(s=='l'&&vS==false){
    if(pS==true){
      pS=false;
    }
    lS=true;
  }
  if(s=='p'&&vS==false){
    if(lS==true){
      lS=false;
    }
    pS=true;
  }
  return;
}
void pressInterruptP(){
  //Serial.println("Zmacknuto");
  podminka('p');
}
void pressInterruptL(){
  //Serial.println("Zmacknuto");
  podminka('l');
}
void setup(){
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(pP), pressInterruptP, RISING);
  attachInterrupt(digitalPinToInterrupt(lP), pressInterruptL, RISING);
  //attachInterrupt(digitalPinToInterrupt(pP), pressInterrupt, FALLING);
}
void loop(){
  Serial.println("Prave tlacitko:");
  Serial.println(pS);
  Serial.println("Leve tlacitko:");
  Serial.println(lS);
  delay(5000);
}