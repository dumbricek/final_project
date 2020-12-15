#include <Arduino.h>
#include "Wire.h"
//#include "I2Cdev.h"
//#include "MPU6050_6Axis_MotionApps20.h"
#define LEDP 5
#define LEDL 6
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 500;
const int lP = 2;
const int pP = 3;
boolean pS = false;
boolean lS = false;
boolean vS = false;
void podminka(char s){
  Serial.println(s);
  if(s=='l'&&vS==false){
    pS=false;
    if(lS==true){
      lS=false;
    }
    else{
      lS=true;
    }
  }
  if(s=='p'&&vS==false){
    lS=false;
    if(pS==true){
      pS=false;
    }
    else{
      pS=true;
    }
  }
  return;
}

void zhasni(){
  digitalWrite(LEDP, LOW);
  digitalWrite(LEDL, LOW);
}

void rozsvit(int p){
  digitalWrite(p, HIGH);
}

void pressInterruptP(){
  podminka('p');
  zhasni();
}

void pressInterruptL(){
  podminka('l');
  zhasni();
}

void setup(){
  pinMode(LEDP, OUTPUT);
  pinMode(LEDL, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(pP), pressInterruptP, RISING);
  attachInterrupt(digitalPinToInterrupt(lP), pressInterruptL, RISING);
  startMillis = millis();
  //attachInterrupt(digitalPinToInterrupt(pP), pressInterrupt, FALLING);
}
void loop(){
  currentMillis = millis();  
  if (currentMillis - startMillis >= period) {
    if(pS){
      digitalWrite(LEDP, !digitalRead(LEDP));  //if so, change the state of the LED.  Uses a neat trick to change the state
    }
    if(lS){
      digitalWrite(LEDL, !digitalRead(LEDL));  //if so, change the state of the LED.  Uses a neat trick to change the state
    }
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}