#include <OneWire.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET     4 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

OneWire ds(5);  // on the pin a 4.7K resistor is necessary

void setup(void) 
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

float Ti[61];

void loop(void) 
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  ds.search(addr);
  OneWire::crc8(addr, 7);

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);       
  delay(1000);     
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         

  for ( i = 0; i < 9; i++) { data[i] = ds.read();  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  
  celsius = (float)raw / 16.0;

display.clearDisplay();
display.setTextSize(1);             
display.setTextColor(WHITE);       

for (int i = 1; i < 60; i++)
{
Ti[i]=Ti[i+1];
display.drawPixel(i+67, Ti[i], WHITE);
}

Ti[60]=31-celsius/2.0; 
display.drawPixel(127, Ti[60], WHITE);
//display.setCursor(1,11);          
//display.println(String(millis()*0.001)+" sec");
display.setCursor(1,1);          
display.println(String(celsius)+(char)247+"C");
display.display();
delay(50);
}
