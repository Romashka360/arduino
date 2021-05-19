#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "TeensyTimerTool.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

using namespace TeensyTimerTool;
PeriodicTimer t1(PIT);
double p1;
int buttonState1 = 0, buttonState2 = 0, buttonState3 = 0, buttonState4 = 0;

void callback()
{
 digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}

void setup()
{
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 pinMode(10, INPUT_PULLUP); 
}

int ff[9]={1,1,1,1,1,1,1,2,1};
int ff2[9];
int fpos = 9;
int tposa[9]={10,9,7,6,5,3,2,1,-1};

void loop()
{  
double p0, p2, p3, p4, p5;

p4 = analogRead(A2);
p5 = analogRead(A1);
p0 = digitalRead(10);
 
display.clearDisplay();
buttonState2 = (p4<300?1:0);
buttonState1 = (p4>700?1:0);
buttonState3 = (p5<300?1:0);
buttonState4 = (p5>700?1:0);

 if (buttonState1 == HIGH)
 {   
 ff[fpos-1] = ff[fpos-1] - 1;
 if (ff[fpos-1] < 1) ff[fpos-1] = 1;
 delay(150);
 } 

 if (buttonState2 == HIGH)
 {   
 ff[fpos-1] = ff[fpos-1] + 1;
 if (ff[fpos-1] > 10) ff[fpos-1] = 10;
 delay(150);
 } 

 if (buttonState3 == HIGH)
 {   
 fpos = fpos - 1;
 if (fpos < 1) fpos = 1;
 delay(150);
 } 

 if (buttonState4 == HIGH)
 {   
 fpos = fpos + 1;
 if (fpos > 9) fpos = 9;
 delay(150);
 } 

 display.setTextSize(1);
 display.setTextColor(WHITE);
 display.setCursor(0,0);
 display.println("f:          . Hz");


float myfreq = 0.0;
float k3 = 0.1;

for (int ii=1; ii<=9; ii+=1)
 {
  display.setCursor(11+(10-tposa[ii-1])*6,0);
  display.println(ff[ii-1]-1);
  ff2[ii-1]=ff[ii-1]-1;
  float f3 = ff2[9-ii];
  myfreq = myfreq + k3*f3;
  k3 = k3*10.0;
 }


float myk = 500000.0/myfreq;
int trh = 14;
int trp = 13+(10-tposa[fpos-1])*6;
  display.fillTriangle(trp, trh-5,
                     trp-3, trh,
                     trp+3, trh, WHITE);

for (int16_t i=2; i<14; i+=1)
{
 display.fillRect(i*6-1, 16, 5, 2, WHITE);
}

if (p0<1)
{    
 pinMode(LED_BUILTIN,OUTPUT);
 t1.begin(callback, myk);
}

  display.display();
  delay(150);
}
