//SAMD21-based datalogger

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>  
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_LSM303_U.h>
#include "RTClib.h"
#include "SdFat.h"
#include <SHT21.h> 
#include <LSM303.h>
//#include <SD.h>
SHT21 sht; 

//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(); // i2c
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

const int chipSelect = 10;
#define SD_CS_PIN 10
#define SPI_CLOCK SD_SCK_MHZ(50)
#define RGB565(r,g,b) (((b&0xF8)<<8)|((g&0xFC)<<3)|((r&0xF8)>>3))

RTC_DS1307 rtc;

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS


#define TFT_CS     7
#define TFT_RST    5
#define TFT_DC     6
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
//#define TFT_CS     13
//#define TFT_RST    10
//#define TFT_DC     11
//#define TFT_SCLK    8
//#define TFT_MOSI     9
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


int i1=0, i2=0, i3=0, i4=0;
#define  myext1 240
float b1[61+myext1], b2[61+myext1], b3[61+myext1], b4[61+myext1];
int b1x[61+myext1], b1y[61+myext1], b1z[61+myext1];
float myB00, nowB, deltaB;

SdFat32 SD;
File32 dataFile;
//File dataFile;


void setup()
{
//  SerialUSB.begin(9600);
//  while (!SerialUSB);
//  SerialUSB.println("Test SerialUSB");

Serial.begin(9600);
//Wire.begin(); SPI.begin();

  mag.enableAutoRange(false);
  mag.begin();
//lsm.begin();
SD.begin(SD_CS_PIN);
  
tft.init(240, 240);
//tft.init(240, 240, SPI_MODE3);

  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(1,40*1, 240,40, RGB565(0x33, 0x88, 0x33));
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(4);
  tft.setCursor(2+10, 40*1+8);
  tft.print("Hello!"); 
  delay(2000);

  rtc.begin(); //rtc.adjust(DateTime(2023, 6, 19, 15, 53, 0));

myB00=0.0;
for (int i=1;i<=100;i++) 
{
  sensors_event_t event2;
  mag.getEvent(&event2);
  myB00=myB00 + sqrt(event2.magnetic.x * event2.magnetic.x + event2.magnetic.y * event2.magnetic.y );
  delay(20);
}
myB00=myB00/100;
}


void loop()
{
String dataString = "";
int myboxh = 60;
int myfontsize = 1;

tft.fillScreen(ST77XX_WHITE);

magneticstorm();

  DateTime now22 = rtc.now();
  float temp = sht.getTemperature();  
  float humidity = sht.getHumidity(); 

    tft.setTextSize(myfontsize);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(2+10, myboxh*4-10);
    tft.print(now22.month()); tft.print("/");
    tft.print(now22.day()); tft.print(" ");
    tft.print(now22.hour(), DEC); tft.print(":");
    tft.print(now22.minute(), DEC); 
    tft.print(" T="); tft.print(temp); tft.print("C");

    int ts = now22.second();  //delay(1000);
    do {now22 = rtc.now();} while(ts == now22.second());
}


void magneticstorm() 
{
float Bbar;
sensors_event_t event2;

  mag.getEvent(&event2);
  deltaB=sqrt(event2.magnetic.x * event2.magnetic.x + event2.magnetic.y * event2.magnetic.y );
  if (deltaB==0) { deltaB=myB00; }
  nowB=deltaB - myB00; //(random(100)-0)/30.0;
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(1, 1);
  tft.print("LSM303D   Bh="); tft.print(deltaB*1000.0); tft.print("nT");
  //SerialUSB.println(nowB);
  
i1++;

for (int i=1;i<=59;i++) b1[61-i]=b1[60-i];
b1[1]=nowB;
for (int i=1;i<=59;i++) b1x[61-i]=b1x[60-i];
b1x[1]=event2.magnetic.x;
for (int i=1;i<=59;i++) b1y[61-i]=b1y[60-i];
b1y[1]=event2.magnetic.y;
for (int i=1;i<=59;i++) b1z[61-i]=b1z[60-i];
b1z[1]=event2.magnetic.z;

if (i1==60)
  {
  i1=0;
  i2++;
  float s=0;
  for (int i=1;i<=60;i++) s=s+b1[i];
  for (int i=1;i<=59;i++) b2[61-i]=b2[60-i];
  b2[1]=s/60;

     s=0;
  for (int i=1;i<=60;i++) s=s+b1x[i];
   float boutx=s/60;

     s=0;
  for (int i=1;i<=60;i++) s=s+b1y[i];
  float bouty=s/60;

     s=0;
  for (int i=1;i<=60;i++) s=s+b1z[i];
  float boutz=s/60;

    DateTime now = rtc.now();
    dataFile = SD.open("storm20230619w.txt", FILE_WRITE);
    dataFile.print(now.year(), DEC); dataFile.print("/");
    dataFile.print(now.month(), DEC); dataFile.print("/");
    dataFile.print(now.day(), DEC); dataFile.print(" ");
    dataFile.print(now.hour(), DEC);dataFile.print(":");
    dataFile.print(now.minute(), DEC);
    dataFile.print(" B= ");dataFile.print(b2[1]);
    dataFile.print(" Bx= ");dataFile.print(boutx);
    dataFile.print(" By= ");dataFile.print(bouty);
    dataFile.print(" Bz= ");dataFile.print(boutz);
    dataFile.println(" ");
    dataFile.close();
  }

if (i2==60)
  {
  i2=0;
  i3++;
  float s=0;
  for (int i=1;i<=60;i++) s=s+b2[i];
  for (int i=1;i<=-1+60*4;i++) b3[1+60*4-i]=b3[60*4-i];
  b3[1]=s/60;
  }

int mybarh = 60;
 
    for (int i=1;i<=60;i++)
    {
    Bbar = (b1[i]-0)*100;
    if (Bbar>mybarh-12) Bbar=mybarh-12;
    if (Bbar<-(mybarh-12)) Bbar=-(mybarh-12);
    tft.fillRect(-1+60*4-i*4,mybarh*1+0, 3,-Bbar-1, ST77XX_GREEN);
    tft.fillRect(240-1,mybarh*1+0, 3,-1, ST77XX_RED);
    
    Bbar = (b2[i]-0)*100;
    if (Bbar>mybarh-12) Bbar=mybarh-12;
    if (Bbar<-(mybarh-12)) Bbar=-(mybarh-12);
    tft.fillRect(-1+60*4-i*4,mybarh*2+0, 3,-Bbar-1, ST77XX_BLACK);
    tft.fillRect(240-1,mybarh*2+0, 3,-1, ST77XX_RED);
    }
    
    for (int i=1;i<=60*4;i++)
    {   
    Bbar = (b3[i]-0)*100;
    if (Bbar>mybarh-12) Bbar=mybarh-12;
    if (Bbar<-(mybarh-12)) Bbar=-(mybarh-12);
    tft.fillRect(-1+60*4-i*1,mybarh*3+0, 1,-Bbar-1, ST77XX_RED);
    tft.fillRect(240-1,mybarh*3+0, 1,-1, ST77XX_BLACK);
    }
}
