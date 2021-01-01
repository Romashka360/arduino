#include <Wire.h>
#include <SPI.h>
#include <Math.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SparkFun_MAG3110.h>
#include "SdFat.h"

#define SD_CS_PIN SS
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MAG3110 mag = MAG3110();
TinyGPSPlus gps;
SoftwareSerial ss(62, 64);
SdFat SD;
File myFile;


void setup(void)
{
  Serial.begin(9600);
  ss.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setRotation(2);
    
  Wire.begin();             //setup I2C bus
  Wire.setClock(400000);    // I2C fast mode, 400kHz
  mag.initialize(); //Initializes the mag sensor
  mag.start();      //Puts the sensor in active mode*mag.x_scale
  
  SD.begin(SD_CS_PIN);
}

unsigned long  timebak, time1;
int adc0=0, adc1=0, adc2=0;
float Voltage0, Voltage1, Voltage2, nVoltage0, nVoltage1, nVoltage2;
float BX, BY, BZ, meanBx, meanBy, meanBz;
float runBx[10]={0,0,0,0,0,0,0,0,0,0};
float runBy[10]={0,0,0,0,0,0,0,0,0,0};
float runBz[10]={0,0,0,0,0,0,0,0,0,0};


void loop(void)
{
int nn, barr;
float sVoltage0=0, sVoltage1=0, sVoltage2=0;
String gps1, gps2, gps3, gps4, gps5;
String wf,sf;
int DD,MM;
double SS, long1;

if(mag.dataReady()) mag.readMag(&adc0, &adc1, &adc2);

int uf = analogRead(A4); //>0< button

if (uf < 512) {
    nn=nn+1;
    sVoltage0=sVoltage0+adc0;
    sVoltage1=sVoltage1+adc1;
    sVoltage2=sVoltage2+adc2;

    nVoltage0 = sVoltage0/nn;
    nVoltage1 = sVoltage1/nn;
    nVoltage2 = sVoltage2/nn;
    }
    else {
      nn=0;
      sVoltage0=0;
      sVoltage1=0;
      sVoltage2=0;
      }

 time1 = millis();

 if (time1 > timebak+100 )
 {
 timebak = millis();

 gps1=String(gps.date.year())+"-"+String(gps.date.month())+"-"+String(gps.date.day());
 gps2=String(gps.time.hour())+":"+String(gps.time.minute())+":"+String(gps.time.second());
 gps5=String(gps.altitude.meters(),0)+" m";

 long1 = gps.location.lat();
 DD = long1;
 long1 -= DD; // remove the degrees from the calculation
 long1 *= 60; // convert to minutes
 MM = long1;
 long1 -= MM; // remove the minuts from the calculation
 long1 *= 60; // convert to seconds
 SS=long1;
 if (long1>0) {
    sf="N";
  } else {
    DD=-DD;
    MM=-MM;
    SS=-SS;
    sf="S";
  }
  
 gps3=String(DD)+ (char)247 +String(MM)+ "'" +String(SS,1)+ "\" "+sf;


 long1 = gps.location.lng();
 DD = long1;
 long1 -= DD; // remove the degrees from the calculation
 long1 *= 60; // convert to minutes
 MM = long1;
 long1 -= MM; // remove the minuts from the calculation
 long1 *= 60; // convert to seconds
 SS=long1;
 if (long1>0) {
    sf="E";
  } else {
    DD=-DD;
    MM=-MM;
    SS=-SS;
    sf="W";
  }

 gps4=String(DD)+ (char)247 +String(MM)+ "'" +String(SS,1)+ "\" "+sf;

  display.clearDisplay();
  display.setTextSize(0);
  display.setTextColor(WHITE);

  display.setCursor(0,32);
  display.print("LAT:"+gps3);
  //float L1 = gps.location.lat();
  //display.println(L1,6);

  display.setCursor(0,40);
  display.print("LONG:"+gps4);
  //float L2 = gps.location.lng();
  //display.println(L2,6);

 BX=(adc0-nVoltage0)*0.100;
 BY=(adc1-nVoltage1)*0.100;
 BZ=(adc2-nVoltage2)*0.100;

  meanBx=0;
  for (int i=0; i <= 8; i++){
  runBx[i] = runBx[i+1];
  meanBx = meanBx+runBx[i];
  }
  runBx[9] = BX;
  BX = (meanBx+runBx[9])/10;


  meanBy=0;
  for (int i=0; i <= 8; i++){
  runBy[i] = runBy[i+1];
  meanBy = meanBy+runBy[i];
  }
  runBy[9] = BY;
  BY = (meanBy+runBy[9])/10;


  meanBz=0;
  for (int i=0; i <= 8; i++){
  runBz[i] = runBz[i+1];
  meanBz = meanBz+runBz[i];
  }
  runBz[9] = BZ;
  BZ = (meanBz+runBz[9])/10;


  float B=sqrt(BX*BX+BY*BY+BZ*BZ);

  tone(7, 3.0+1000*log10(abs(B))); // Output to buzzer

    barr = 10*log10(abs(BX)+1)+1;
    if (BX>=0){display.fillRect(64, 26, barr, 2, WHITE);} else {display.fillRect(64-barr, 26, barr, 2, WHITE);}

    barr = 10*log10(abs(BY)+1)+1;
    if (BY>=0){display.fillRect(64, 28, barr, 2, WHITE);} else {display.fillRect(64-barr, 28, barr, 2, WHITE);}

    barr = 10*log10(abs(BZ)+1)+1;
    if (BZ>=0){display.fillRect(64, 30, barr, 2, WHITE);} else {display.fillRect(64-barr, 30, barr, 2, WHITE);}


  myFile = SD.open("test2019.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(BZ,1);
    myFile.println(gps1);
    myFile.close();
    Serial.println("done.");
    wf="+";
  } else {
    Serial.println("error opening test.txt");
    wf="?";
  }

  display.setCursor(0,56);
  display.print(wf+"");
  display.print(gps1);
  display.print(" ");
  display.println(gps2);

  display.setCursor(0,0);
  display.print("BX:");
  display.print(BX,1);
  display.println(" uT");

  display.setCursor(0,8);
  display.print("BY:");
  display.print(BY,1);
  display.println(" uT");

  display.setCursor(0,16);
  display.print("BZ:");
  display.print(BZ,1);
  display.println(" uT");

  display.display();
 }
 while (ss.available() > 0)
   gps.encode(ss.read());
}
