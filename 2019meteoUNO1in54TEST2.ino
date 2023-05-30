#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <GxEPD2_AVR_BW.h>
#include <GxEPD2_AVR_3C.h>
#include <Fonts/Org_01.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "SdFat.h"

static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;   // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);   // The serial connection to the GPS device

//#define SD_CS_PIN SS
#define SD_CS_PIN 8
SdFat SD;
File myFile;

#include "bitmaps/Bitmaps200x200.h" // 1.54" b/w
//#include "bitmaps/Bitmaps128x250.h" // 2.13" b/w
//#include "bitmaps/Bitmaps176x264.h" // 2.7"  b/w

GxEPD2_AVR_BW display(GxEPD2::GDEP015OC1, /*CS=*/ SS, /*DC=*/ 9, /*RST=*/ 8, /*BUSY=*/ 7);
//GxEPD2_AVR_BW display(GxEPD2::GDE0213B1, /*CS=*/ SS, /*DC=*/ 9, /*RST=*/ 8, /*BUSY=*/ 7);
//GxEPD2_AVR_BW display(GxEPD2::GDEW027W3, /*CS=*/ SS, /*DC=*/ 9, /*RST=*/ 8, /*BUSY=*/ 7);

//#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bme; // I2C

unsigned long time_start, time1h, time_now, time_tot_start;
//int ppp[61],ppp1h[61];
String gps1, gps2, gps5;

void setup()
{
  Serial.begin(9600);
  display.init();

  ss.begin(GPSBaud);
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN)) { Serial.println("initialization failed!"); return; }
  Serial.println("initialization done.");
  
 
  if (!bme.begin()) 
  {
    Serial.println("Could not find a valid BOSCH sensor, check wiring!");
    while (1);
  }

  helloWorld();
  time_start = millis();
  time1h = millis();
  time_tot_start = millis();
}

double Tem, Gas, Hum, Pre;
double Tem100, Gas100, Hum100, Pre100;


void loop()
{

  time_now = millis();
  double s1h = 0.0;
  double s1 = (time_now - time1h)/1000.0; 

  time_now = millis();
  double s2 = (time_now - time_start)/1000.0; 
  if (s2 > 15)
  {
  double time_tot = (millis() - time_tot_start) / 1000.0;


    gps1=String(gps.date.year())+"-"+String(gps.date.month())+"-"+String(gps.date.day());
    gps2=String(gps.time.hour())+":"+String(gps.time.minute())+":"+String(gps.time.second());
    gps5=String(gps.altitude.meters(),0)+" m";
  

    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure());
    Serial.println(" Pa");


    myFile = SD.open("my2019TEST2.txt", FILE_WRITE);
    
    if (myFile) {
      myFile.print( gps1+"  "+gps2 );
      myFile.println(" ");
      //delay(50); 
    } 
    else {
      Serial.println("error opening .txt");
    }

    myFile.close();

    

Tem = bme.readTemperature();
Tem100 = Tem*100.0;
Pre = bme.readPressure()/101325.0144354*760.0;
Pre100 = Pre*100.0;
Hum = 22; Hum100 = 22;


  char time_string1[] = {'T', '=', '0', '0', '.', '0', '0', 'C','\0'};
  time_string1[2] = int(Tem) / 10 %10  + '0';
  time_string1[3] = int(Tem) / 1 %10  + '0';
  time_string1[5] = int(Tem100) / 10 %10  + '0';
  time_string1[6] = int(Tem100) / 1 %10  + '0';

  char time_string3[] = {'P', '=', '0', '0', '0', '.', '0', 'm', 'm', 'H', 'g', ' ', '\0'};
  time_string3[2] = int(Pre) / 100 %10  + '0';
  time_string3[3] = int(Pre) / 10 %10  + '0';
  time_string3[4] = int(Pre) / 1 %10  + '0';
  time_string3[6] = int(Pre100) / 10 %10  + '0';

  char time_string[] = {'0', '0', '0', ':', '0', '0', ':', '0', '0', '\0'};
  int th = floor(time_tot/3600.0);
  int tmin = floor((time_tot - th*3600.0)/60.0);
  int tsec = floor(time_tot - th*3600.0 - tmin*60.0);
  time_string[0] = th / 100 %10  + '0';
  time_string[1] = th / 10 %10  + '0';
  time_string[2] = th / 1 %10  + '0';
  time_string[4] = tmin / 10 %10  + '0';
  time_string[5] = tmin / 1 %10  + '0';
  time_string[7] = tsec / 10 %10  + '0';
  time_string[8] = tsec / 1 %10  + '0';

  display.setPartialWindow(0, 0, 200, 22); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(50, 5);
    display.println(time_string1);
//    display.setCursor(90, 15);
//    display.println(time_string2);
    display.setCursor(130, 5);
    display.println(time_string3);
//    display.setCursor(170, 15);
//    display.println(time_string4);
    display.setCursor(1, 15);
    display.println(time_string);
  }
  while (display.nextPage());

//--------------------------------------------------------------------------------------------
  display.setPartialWindow(4, 22, 195, 178); // x,y,width,height
  display.firstPage();
  do
  {
  display.fillScreen(GxEPD_WHITE);
  for (int i = 1; i <= 60; i++)
  {
  int barh=75+floor((755.0/10.0-750.0)*5.0);
  display.fillRect(i*3+6, 200-barh, 2, barh, GxEPD_BLACK); //x, y, w, h, GxEPD_BLACK
  }
  }
  while (display.nextPage());


  display.setPartialWindow(0, 0, 50, 5); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(1, 5);
    display.println(" 60m");
  }
  while (display.nextPage());
//---------------------------------------------------------------------------------------------
  display.setPartialWindow(4, 22, 195, 178); // x,y,width,height
  display.firstPage();
  do
  {
  display.fillScreen(GxEPD_WHITE);
  for (int i = 1; i <= 60; i++)
  {
  int barh=75+floor((755.0/10.0-750.0)*5.0);
  display.fillRect(i*3+6, 200-barh, 2, barh, GxEPD_BLACK); //x, y, w, h, GxEPD_BLACK
  }
  }
  while (display.nextPage());


  display.setPartialWindow(0, 0, 50, 5); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(1, 5);
    display.println(" 60h");
  }
  while (display.nextPage());
//--------------------------------------------------------------------------------------------
  time_start = time_now;
 }

while (ss.available() > 0)
    gps.encode(ss.read());

 
}

//==============================================================================================

void helloWorld()
{
  display.setFullWindow();
  display.setRotation(1);
  display.setFont(&Org_01);
  display.setTextColor(GxEPD_BLACK);
  uint16_t x = 1;//(display.width() - 160) / 2;
  uint16_t y = 5;//display.height() / 2;
  Serial.println(display.width());
  Serial.println(display.height());
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.println(F("helloWorld"));
  }
  while (display.nextPage());
//-------------------------------------------------------------------------------------
  display.setPartialWindow(0, 22, 3, 150); // x,y,width,height
  display.firstPage();
  do
  {
  display.fillScreen(GxEPD_WHITE);
  display.fillRect(0, 200-75, 3, 1, GxEPD_BLACK); //x, y, w, h, GxEPD_BLACK
  display.fillRect(0, 200-75-50, 3, 1, GxEPD_BLACK); //x, y, w, h, GxEPD_BLACK
  display.fillRect(0, 200-75+50, 3, 1, GxEPD_BLACK); //x, y, w, h, GxEPD_BLACK
  }
  while (display.nextPage());
}
