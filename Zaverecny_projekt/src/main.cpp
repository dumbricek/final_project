#include <Arduino.h>
#define LED 13
#define LED1 7
#define BUTTON 12
#define BUTTON1 10

boolean on = false;
boolean on1 = false;
int buttonState1 = 0;
int buttonState = 0;
int state =0;
int state1 = 0;
void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(LED1, OUTPUT);
  // put your setup code here, to run once:
}

void loop() {
  /*if(digitalRead(BUTTON)==1){
    Serial.println(state);
    state++;
  }
  delay(500);*/
  buttonState = digitalRead(BUTTON);
  buttonState1 = digitalRead(BUTTON1);
  if (buttonState == HIGH){
    if(on==true){
      on=false;
    }
    else if(on==false)
    {
      on=true;
    }
  }

  if (buttonState1 == HIGH){
    if(on1==true){
      on1=false;
    }
    else if(on1==false)
    {
      on1=true;
    }
  }
  if (on==true){
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
  }
  if (on==false){
    digitalWrite(LED, LOW);
  }
  
  if (on1==true){
    digitalWrite(LED1, HIGH);
    delay(500);
    digitalWrite(LED1, LOW);
  }
  if (on1==false){
    digitalWrite(LED1, LOW);
  }

  delay(500);
  


  // put your main code here, to run repeatedly:
}