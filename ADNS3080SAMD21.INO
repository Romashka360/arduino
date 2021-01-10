#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>

#define ADNS3080_FRAME_CAPTURE         0x13
#define RGB565(r,g,b) (((b&0xF8)<<8)|((g&0xFC)<<3)|((r&0xF8)>>3))

Adafruit_ST7735 tft=Adafruit_ST7735(/*TFT_CS*/11,/*TFT_DC*/10,/*TFT_MOSI*/13,/*TFT_SCLK*/12,/*TFT_RST*/9);
SPISettings spiSettings(2e6, MSBFIRST, SPI_MODE3); // 2 MHz, mode 3

static const uint8_t RESET_PIN = 5;
static const uint8_t SS_PIN = 6;
float time0, time1, s;


void setup() 
{
tft.initR(INITR_MINI160x80);
SPI.begin();
pinMode(SS_PIN, OUTPUT);
pinMode(RESET_PIN, OUTPUT);
time0 = millis();
}

uint16_t ADNS3080IMG[1537];

void loop() 
{
spiWrite(ADNS3080_FRAME_CAPTURE, 0x83); //capture frame 
delayMicroseconds(1510); //~3 frames

for (int i1 = 0 ; i1 < 50 ; i1++)
{
for (int i2 = 0 ; i2 < 30 ; i2++)
{
uint8_t regValue = spiRead(ADNS3080_FRAME_CAPTURE);
uint8_t pixelValue = regValue << 2; 
float mycol565 = (pixelValue==252?RGB565(0, 255, 255):RGB565(252-pixelValue, 252-pixelValue, 252-pixelValue));
s=2;
tft.fillRect(30*s-i2*s,i1*s, s,s, mycol565);
}
}

  time1 = millis();
  tft.fillRect(1,150, 80,10, ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_BLACK);
  tft.setCursor(2, 151);
  tft.print((time1-time0)*0.001);  
  tft.setCursor(50, 151);
  tft.print("sec");  
}

//=================================================
void spiWrite(uint8_t reg, uint8_t data) {
  spiWrite(reg, &data, 1);
}

void spiWrite(uint8_t reg, uint8_t *data, uint8_t length) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(SS_PIN, LOW);

  SPI.transfer(reg | 0x80); // Indicate write operation
  delayMicroseconds(75); // Wait minimum 75 us in case writing to Motion or Motion_Burst registers
  SPI.transfer(data, length); // Write data

  digitalWrite(SS_PIN, HIGH);
  SPI.endTransaction();
}

uint8_t spiRead(uint8_t reg) {
  uint8_t buf;
  spiRead(reg, &buf, 1);
  return buf;
}

void spiRead(uint8_t reg, uint8_t *data, uint8_t length) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(SS_PIN, LOW);

  SPI.transfer(reg); // Send register address
  delayMicroseconds(75); // Wait minimum 75 us in case writing to Motion or Motion_Burst registers
  memset(data, 0, length); // Make sure data buffer is 0
  SPI.transfer(data, length); // Write data

  digitalWrite(SS_PIN, HIGH);
  SPI.endTransaction();
}
