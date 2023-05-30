#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "SparkFunBME280.h"
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/Org_01.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define GxEPD2_DRIVER_CLASS GxEPD2_270 
#define MAX_DISPLAY_BUFFER_SIZE 32768ul // e.g., up to 96k
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

GxEPD2_BW<GxEPD2_270, MAX_HEIGHT(GxEPD2_270)> display(GxEPD2_270(/*CS=*/ 10, /*DC=*/ 11, /*RST=*/ 12, /*BUSY=*/ 13)); // GDEW027W3 176x264, EK79652 (IL91874)
BME280 mySensorB;

unsigned long mytime_start, mytime_now, mytime_tot_start;

void setup() 
{
display.init();
display.setTextColor(GxEPD_BLACK);
helloWorld();

Wire.begin();
mySensorB.setI2CAddress(0x76);
mySensorB.beginI2C();

mytime_start = millis()+60;
mytime_tot_start = millis();
}

void loop()
{
  mytime_now = millis();
  double s2 = (mytime_now - mytime_start)/1000.0; 
  
  if (s2 > 5)
  {
  mytime_start = mytime_now;
  
  double mytime_tot = (millis() - mytime_tot_start) / 1000.0;
  double Tem = mySensorB.readTempC();
  double Tem100 = Tem*100.0;
  double Pre = mySensorB.readFloatPressure()/101325.0144354*760.0;
  double Pre100 = Pre*100.0;
  double Hum = mySensorB.readFloatHumidity(); 
  double Hum100 = Hum*100.0;

  char time_string1[] = {'T', '=', '0', '0', '.', '0', '0', 'C','\0'};
  time_string1[2] = int(Tem) / 10 %10  + '0';
  time_string1[3] = int(Tem) / 1 %10  + '0';
  time_string1[5] = int(Tem100) / 10 %10  + '0';
  time_string1[6] = int(Tem100) / 1 %10  + '0';

  char time_string2[] = {'H', '=', '0', '0', '.', '0', '%','\0'};
  time_string2[2] = int(Hum) / 10 %10  + '0';
  time_string2[3] = int(Hum) / 1 %10  + '0';
  time_string2[5] = int(Hum100) / 10 %10  + '0';

  char time_string3[] = {'P', '=', '0', '0', '0', '.', '0', 'm', 'm', 'H', 'g', ' ', '\0'};
  time_string3[2] = int(Pre) / 100 %10  + '0';
  time_string3[3] = int(Pre) / 10 %10  + '0';
  time_string3[4] = int(Pre) / 1 %10  + '0';
  time_string3[6] = int(Pre100) / 10 %10  + '0';

  char time_string[] = {'0', '0', '0', '0', ':', '0', '0', ':', '0', '0', '\0'};
  int th = floor(mytime_tot/3600.0);
  int tmin = floor((mytime_tot - th*3600.0)/60.0);
  int tsec = floor(mytime_tot - th*3600.0 - tmin*60.0);
  time_string[0] = th / 1000 %10  + '0';
  time_string[1] = th / 100 %10  + '0';
  time_string[2] = th / 10 %10  + '0';
  time_string[3] = th / 1 %10  + '0';
  time_string[5] = tmin / 10 %10  + '0';
  time_string[6] = tmin / 1 %10  + '0';
  time_string[8] = tsec / 10 %10  + '0';
  time_string[9] = tsec / 1 %10  + '0';

  display.setPartialWindow(50, 30, 190, 140); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(50, 60);
    display.println(time_string1);

    display.setCursor(50, 90);
    display.println(time_string3);

    display.setCursor(50, 120);
    display.println(time_string2);

    display.setCursor(50, 150);
    display.println(time_string);
  }
  while (display.nextPage());
 }
}


void helloWorld()
{
  display.setFullWindow();
  display.setRotation(3);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(1, 15);
    display.println(F("BME280"));
  }
  while (display.nextPage());
}
