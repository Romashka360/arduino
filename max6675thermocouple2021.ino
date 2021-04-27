#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>
#include "max6675.h"
 
MAX6675 thermocouple1(/*thermoCLK=*/ 12, /*thermoCS=*/ 11, /*thermoDO=*/ 6);
MAX6675 thermocouple2(/*thermoCLK=*/ 12, /*thermoCS=*/ 10, /*thermoDO=*/ 9);
Adafruit_ST7789 tft = Adafruit_ST7789(/*TFT_CS=*/ 0, /*TFT_DC=*/ 5, /*TFT_RST=*/ 13);

#define RGB565(r,g,b) (((b&0xF8)<<8)|((g&0xFC)<<3)|((r&0xF8)>>3))

float Ti1[240],Ti2[240],Ti3[240];
float TIME1, TIME2, TIME0, A000;
uint16_t  L0=240;


void setup(void) 
{
SPI.begin();
tft.init(240, 240, SPI_MODE3);
tft.fillScreen(ST77XX_BLACK);
for (int i = 1; i <= L0; i++)
{
Ti1[i]=150;
Ti2[i]=150;
Ti3[i]=150;
}

while (floor(millis()/1000)*1000<millis()){TIME0 = millis();}
TIME2 = millis();
}


void loop() 
{
while (millis()<TIME2+1000){TIME1 = millis();}

tft.fillRect(0,0, L0,180, ST77XX_BLACK);
tft.setTextSize(2);
tft.setTextColor(ST77XX_WHITE);
tft.setCursor(2, 2);
tft.print("t");
tft.setCursor(40, 2);
tft.print((TIME1-TIME0)/1000);

tft.fillRect(1,185, L0,30, ST77XX_WHITE);
tft.setTextColor(ST77XX_BLACK);
 
tft.setCursor(2, 191);
tft.print("T1"); 
tft.setCursor(140, 191);
tft.print("T2");
tft.setTextColor(ST77XX_RED);  
tft.setCursor(42, 191);
tft.print(thermocouple1.readCelsius()*10/10); 
tft.setTextColor(ST77XX_BLUE);
tft.setCursor(180, 191);
tft.print(thermocouple2.readCelsius());


for (int i = 1; i < L0; i++)
{
Ti1[i]=Ti1[i+1];
tft.fillRect(i, Ti1[i], 3,3, ST77XX_RED);
}

Ti1[L0]=150-thermocouple1.readCelsius()/2.0; 
tft.fillRect(L0, Ti1[L0], 3,3, ST77XX_RED);


for (int i = 1; i < L0; i++)
{
Ti2[i]=Ti2[i+1];
tft.fillRect(i, Ti2[i], 3,3, ST77XX_BLUE);
}

Ti2[L0]=150-thermocouple2.readCelsius()/2.0; 
tft.fillRect(L0, Ti2[L0], 3,3, ST77XX_BLUE);


for (int i = 1; i < L0; i++)
{
Ti3[i]=Ti3[i+1];
tft.fillRect(i, Ti3[i], 3,3, ST77XX_YELLOW);
}

Ti3[L0]=150-A000/0.1; 
tft.fillRect(L0, Ti3[L0], 3,3, ST77XX_WHITE);


TIME2 = TIME1;
delay(100);
}

