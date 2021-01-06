#include <Arduino.h>
#include "Wire.h"
#define LEDP 5
#define LEDL 6
const int MPU = 0x68; // MPU6050 I2C address
float AccX, GyroX, accAngleX, gyroAngleX;
float roll, rollTmp, avRoll;
float AccErrorX, GyroErrorX;
float elapsedTime, currentTime, previousTime;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 500;
const int lP = 2;
const int pP = 3;
boolean pS = false;
boolean lS = false;
boolean stateL = false;
boolean stateL2 = false;
boolean stateP = false;
boolean stateP2 = false;

void zhasni(){
  digitalWrite(LEDP, LOW);
  digitalWrite(LEDL, LOW);
}

void podminka(char s){
  zhasni();
  if(s=='l'){
    pS=false;
    lS=!lS;
  }
  if(s=='p'){
    lS=false;
    pS=!pS;
  }
  return;
}

void pressInterruptP(){
  podminka('p');
}

void pressInterruptL(){
  podminka('l');
}

void setup(){
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  pinMode(LEDP, OUTPUT);
  pinMode(LEDL, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(pP), pressInterruptP, RISING);
  attachInterrupt(digitalPinToInterrupt(lP), pressInterruptL, RISING);
  startMillis = millis();
}
void loop(){
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  currentMillis = millis();  
  if (currentMillis - startMillis >= period) {
    if(pS){
      digitalWrite(LEDP, !digitalRead(LEDP));  //if so, change the state of the LED.  Uses a neat trick to change the state
      if(roll > rollTmp + 1 && !stateP){
        stateP2 = false;
        stateP = true;
      }
      else if (roll < rollTmp -1 && stateP){
        stateP2 = true;
        stateP = false;
      }
      else if ((roll < rollTmp +0.1 && roll>rollTmp - 0.1)&&stateP2){
        stateP2=false;
        pS=false;
        zhasni();
        Serial.println("nakloněno doprava");
      }
      rollTmp = roll;
    }
    if(lS){
      digitalWrite(LEDL, !digitalRead(LEDL));  //if so, change the state of the LED.  Uses a neat trick to change the state
      if(roll < rollTmp - 1 && !stateL){
        stateL2 = false;
        stateL = true;
        Serial.println(roll);
        Serial.println(rollTmp);
      }
      else if (roll > rollTmp +1 && stateL){
        stateL2 = true;
        stateL = false;
        Serial.println(roll);
        Serial.println(rollTmp);
      }
      else if ((roll < rollTmp +0.1 && roll>rollTmp - 0.1)&&stateL2){
        stateL2=false;
        lS=false;
        zhasni();
        Serial.println("nakloněno doleva");
      }
      rollTmp = roll;
    }
    else{
      rollTmp = roll;
    }
    
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.

  }
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  // Calculating Roll from the accelerometer data
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  // Correct the outputs with the calculated error values
  GyroX = GyroX+2.93 ; // GyroErrorX ~(-2.93)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
}