#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

void setup(void)
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(2);
}

void loop(void)
{
  float u1 = 1024-analogRead(A1);  
  float u2 = analogRead(A0)+1;  
 
  float bar1 = 1+128*log10(1+9.9*u1/1024.0);
  float bar2 = 1+128*log10(1+9.9*u2/1024.0);
   
  display.fillRect(0, 10, bar1, 2, WHITE);
  display.fillRect(0, 26, bar2, 2, WHITE);
  
  display.setTextSize(0);
  display.setTextColor(WHITE);
  
  display.setCursor(0,0);
  display.print("+:");
  display.print(u1,0);
  
  display.setCursor(0,16);
  display.print("-:");
  display.print(u2,0);
  
  display.display();
  delay(30);
  display.clearDisplay();
}
