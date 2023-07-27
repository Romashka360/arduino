#include "si5351.h"
#include "Wire.h"
#include <ERM19264_UC1609.h>

#define MYLCDHEIGHT 64
#define MYLCDWIDTH  192
#define VbiasPOT 0x49 // contrast range 0x00 to 0xFF
#define LCDCONTRAST 0x49 
#define CD 11  
#define RST 12 
#define CS 10 
#define SCLK 7 
#define DIN  6 

ERM19264_UC1609  mylcd(CD, RST, CS, SCLK, DIN); 
Si5351 si5351;

uint8_t  screenBuffer[(MYLCDWIDTH * (MYLCDHEIGHT/8))+1]; // 1536 +1 bytes

float kHzStateF(float kHzState00) 
{
float kHzPosF[] = {0, 181, 322, 413, 509, 559, 606, 640, 660, 686};
float kHzState33;
if (kHzState00 < kHzPosF[0]+10) {kHzState33=0;}else
if (kHzState00 < kHzPosF[1]+10) {kHzState33=1;}else
if (kHzState00 < kHzPosF[2]+10) {kHzState33=2;}else
if (kHzState00 < kHzPosF[3]+10) {kHzState33=3;}else
if (kHzState00 < kHzPosF[4]+10) {kHzState33=4;}else
if (kHzState00 < kHzPosF[5]+10) {kHzState33=5;}else
if (kHzState00 < kHzPosF[6]+10) {kHzState33=6;}else
if (kHzState00 < kHzPosF[7]+10) {kHzState33=7;}else
if (kHzState00 < kHzPosF[8]+10) {kHzState33=8;}else
if (kHzState00 < kHzPosF[9]+10) {kHzState33=9;}; 
return kHzState33;
}


void setup() 
{
//  delay(50);
  mylcd.LCDbegin(LCDCONTRAST); 
  mylcd.LCDFillScreen(0x00, 0); 
  mylcd.setTextColor(FOREGROUND); 
  mylcd.setFontNum(UC1609Font_Default);
  mylcd.LCDrotate(UC1609_ROTATION_FLIP_ONE);
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
}


void loop() 
{
String myStr;
float kHzState0;
int kHzState1 = 0, kHzState2 = 0, kHzState3 = 0, kHzState4 = 0, kHzState5 = 0; 

kHzState0 = analogRead(A1);
kHzState5 = kHzStateF(kHzState0);
kHzState0 = analogRead(A2);
kHzState4 = kHzStateF(kHzState0);
kHzState0 = analogRead(A3);
kHzState3 = kHzStateF(kHzState0);
kHzState0 = analogRead(A4);
kHzState2 = kHzStateF(kHzState0);

si5351.update_status();
delay(50);

MultiBuffer myStruct; 
mylcd.LCDinitBufferStruct(&myStruct, screenBuffer, MYLCDWIDTH, MYLCDHEIGHT, 0, 0);  
mylcd.ActiveBuffer = &myStruct;    
mylcd.LCDclearBuffer();   
mylcd.setTextColor(FOREGROUND);
mylcd.setTextSize(2);
myStr = + "0" + String(kHzState2)+ "." + String(kHzState3)+String(kHzState4)+String(kHzState5)+"00 MHz";
si5351.set_freq(kHzState2*100000000+kHzState3*10000000+kHzState4*1000000+kHzState5*100000, SI5351_CLK0);
mylcd.setCursor(20, 5);
mylcd.print("SI5351 (c)2023");
mylcd.setCursor(20, 35);
mylcd.print(myStr);
mylcd.LCDupdate(); 
delay(50);
}
