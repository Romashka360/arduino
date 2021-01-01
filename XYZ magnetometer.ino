#include <Wire.h>
#include <SPI.h>
#include <Math.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

Adafruit_ADS1115 ads(0x48);

float Voltage0, Voltage1, Voltage2, nVoltage0, nVoltage1, nVoltage2;
float BX, BY, BZ, meanBx, meanBy, meanBz;
float runBx[10]={0,0,0,0,0,0,0,0,0,0};
float runBy[10]={0,0,0,0,0,0,0,0,0,0};
float runBz[10]={0,0,0,0,0,0,0,0,0,0};


void setup(void) 
{
  //set 16-bit analog-to-digital converter
  ads.begin();
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  

  //set oled display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  delay(2000);
  display.clearDisplay();
}


void loop(void) 
{
int nn, barr;
float adc0=0, adc1=0, adc2=0;  
float sVoltage0=0, sVoltage1=0, sVoltage2=0;
      
  adc0 = ads.readADC_SingleEnded(2); //X-axis
  adc1 = ads.readADC_SingleEnded(1); //Y-axis
  adc2 = ads.readADC_SingleEnded(0); //Z-axis
 
  Voltage0 = (adc0 * 0.1875)/1000; //  2/3x gain
  Voltage1 = (adc1 * 0.1875)/1000; //  2/3x gain
  Voltage2 = (adc2 * 0.1875)/1000; //  2/3x gain

int uf = analogRead(A8); //>0< button

  if (uf < 512) {
    nn=nn+1;
    sVoltage0=sVoltage0+Voltage0;
    sVoltage1=sVoltage1+Voltage1;
    sVoltage2=sVoltage2+Voltage2;
    
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
   
   BX=(Voltage0-nVoltage0)/0.0050*100;  
   BY=(Voltage1-nVoltage1)/0.0050*100;  
   BZ=(Voltage2-nVoltage2)/0.0050*100;  

 
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

  tone(10, 300.0+1000*log10(abs(B))); // Output to buzzer 

    barr = 10*log10(abs(BX)+1)+1; 
    if (BX>=0){display.fillRect(64, 26, barr, 2, WHITE);} else {display.fillRect(64-barr, 26, barr, 2, WHITE);}
   
    barr = 10*log10(abs(BY)+1)+1; 
    if (BY>=0){display.fillRect(64, 28, barr, 2, WHITE);} else {display.fillRect(64-barr, 28, barr, 2, WHITE);}

    barr = 10*log10(abs(BZ)+1)+1; 
    if (BZ>=0){display.fillRect(64, 30, barr, 2, WHITE);} else {display.fillRect(64-barr, 30, barr, 2, WHITE);}
   
  display.setTextSize(0);
  display.setTextColor(WHITE);
  
  display.setCursor(0,0);
  display.print("BX:");
  display.print(BX,0);
  display.println(" uT");

  display.setCursor(0,8);
  display.print("BY:");
  display.print(BY,0);
  display.println(" uT");

  display.setCursor(0,16);
  display.print("BZ:");
  display.print(BZ,0);
  display.println(" uT");
 
    display.display();
    delay(30);
    display.clearDisplay();
}
