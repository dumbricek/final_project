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
  Wire.begin();                      // Inicializace komunikace
  Wire.beginTransmission(MPU);       // Start komunikace s čipem MPU-6050 // MPU=0x68
  Wire.write(0x6B);                  // Komunikování s registrem 6B
  Wire.write(0x00);                  // Udělá reset - vloží 0 do registru 6B
  Wire.endTransmission(true);        // Ukončí přenos
  pinMode(LEDP, OUTPUT);             // 
  pinMode(LEDL, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(pP), pressInterruptP, RISING); // při zmáčknutí tlačítka se zapne funkce pressInterruptP
  attachInterrupt(digitalPinToInterrupt(lP), pressInterruptL, RISING); 
  startMillis = millis(); // 
}
void loop(){
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  currentMillis = millis();  
  if (currentMillis - startMillis >= period) {
    if(pS){
      digitalWrite(LEDP, !digitalRead(LEDP));  //změní stav pravé LE diody
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
      }
      rollTmp = roll;
    }
    if(lS){
      digitalWrite(LEDL, !digitalRead(LEDL));  //if so, change the state of the LED.  Uses a neat trick to change the state
      if(roll < rollTmp - 1 && !stateL){
        stateL2 = false;
        stateL = true;
      }
      else if (roll > rollTmp +1 && stateL){
        stateL2 = true;
        stateL = false;
      }
      else if ((roll < rollTmp +0.1 && roll>rollTmp - 0.1)&&stateL2){
        stateL2=false;
        lS=false;
        zhasni();
      }
      rollTmp = roll;
    }
    else{
      rollTmp = roll;
    }
    startMillis = currentMillis;  // uložení času změny LE diody
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
  elapsedTime = (currentTime - previousTime) / 1000; // Dělení tisícem na převod na sekundy
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroX = GyroX+2.93 ; // GyroErrorX ~(-2.93)
  // Teď jsou surové data uložený ve stupních za sekudnu, takže je potřebujem vynásobit sekundami, abychom dostali velikost úhlu
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
}
//funkce pro zjištění odchylky mezi jednotlivými měřeními
/*void calculate_IMU_error() {
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}*/