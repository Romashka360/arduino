#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <GxEPD2_AVR_BW.h>
//#include <GxEPD2_AVR_3C.h>
#include <Fonts/Org_01.h>

//#include "bitmaps/Bitmaps200x200.h" // GDEP015OC1 1.54" b/w
//#include "bitmaps/Bitmaps128x250.h" // GDE0213B1 2.13" b/w
//#include "bitmaps/Bitmaps176x264.h" // GDEW027W3 2.7"  b/w

#if defined(ESP8266)
//under construction
#elif defined(ESP32)
GxEPD2_AVR_BW display(GxEPD2::GDEP015OC1, /*CS=*/ 5, /*DC=*/ 13, /*RST=*/ 12, /*BUSY=*/ 14);
#elif defined(ARDUINO_ARCH_SAMD)
//under construction
#elif defined(ARDUINO_AVR_MEGA2560)
//under construction
#else
GxEPD2_AVR_BW display(GxEPD2::GDEP015OC1, /*CS=*/ 10, /*DC=*/ 9, /*RST=*/ 8, /*BUSY=*/ 7);
#endif

Adafruit_BME280 bme; // I2C

unsigned long mytime_start, mytime_now, mytime_tot_start;


void setup()
{
  Serial.begin(9600);
  display.init();
  bme.begin();
  helloWorld();

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
  double Tem = bme.readTemperature();
  double Tem100 = Tem*100.0;
  double Pre = bme.readPressure()/101325.0144354*760.0;
  double Pre100 = Pre*100.0;
  double Hum = bme.readHumidity(); 
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

  display.setPartialWindow(50, 50, 100, 50); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(50, 50);
    display.println(time_string1);

    display.setCursor(50, 65);
    display.println(time_string3);

    display.setCursor(50, 80);
    display.println(time_string2);

    display.setCursor(50, 95);
    display.println(time_string);
  }
  while (display.nextPage());
 }
}


void helloWorld()
{
  display.setFullWindow();
  display.setRotation(1);
  //display.setFont(&Org_01);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(1, 5);
    display.println(F("BME280"));
  }
  while (display.nextPage());
}
