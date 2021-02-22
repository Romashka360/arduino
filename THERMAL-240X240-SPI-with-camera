#include <Adafruit_GFX.h>     
#include <Adafruit_ST7735.h>  
#include <Adafruit_ST7789.h>  
#include <SPI.h>
#include <Wire.h>
#include "MLX90640_API.h" //github.com/sparkfun/SparkFun_MLX90640_Arduino_Example
#include "MLX90640_I2C_Driver.h"

#define TA_SHIFT 8 //Default shift for MLX90640 in open air

const byte MLX90640_address = 0x33;
static float mlx90640To[768];
paramsMLX90640 mlx90640;

#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_CONFIGURATION_BITS    0x0A
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_MOTION_BURST          0x50

#define REG3080_Shutter_Lower                        0x0E
#define REG3080_Shutter_Upper                        0x0F
#define REG3080_Frame_Period_Lower                   0x10
#define REG3080_Frame_Period_Upper                   0x11
#define REG3080_Frame_Period_Max_Bound_Lower         0x19
#define REG3080_Frame_Period_Max_Bound_Upper         0x1A
#define REG3080_Frame_Period_Min_Bound_Lower         0x1B
#define REG3080_Frame_Period_Min_Bound_Upper         0x1C
#define REG3080_Shutter_Max_Bound_Lower              0x1D
#define REG3080_Shutter_Max_Bound_Upper              0x1E
#define REG3080_Extended_Config                      0x0B
#define REG3080_Configuration_bits                   0x0A

#define RGB565(r,g,b) (((b&0xF8)<<8)|((g&0xFC)<<3)|((r&0xF8)>>3))

#define TFT_CS     0
#define TFT_RST    6  
#define TFT_DC     5

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS,  TFT_DC, TFT_RST);
SPISettings spiSettings(2e6, MSBFIRST, SPI_MODE3); // 2 MHz, mode 3

static const uint8_t RESET_PIN = 9;
static const uint8_t SS_PIN = 13;
float time0, time1, s;
uint16_t TIME1, TIME2, TIME0;


void setup(void) 
{
  Wire.begin();
  Wire.setClock(800000); //Increase I2C clock speed to 400kHz
  Wire.beginTransmission((uint8_t)MLX90640_address);
  uint16_t eeMLX90640[832];
  MLX90640_DumpEE(MLX90640_address, eeMLX90640);
  MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  MLX90640_SetRefreshRate(MLX90640_address, 0x04); //Set rate to 2Hz
  
SPI.begin();
tft.init(240, 240, SPI_MODE3);
tft.fillScreen(ST77XX_BLACK);
pinMode(SS_PIN, OUTPUT);
pinMode(RESET_PIN, OUTPUT);
delayMicroseconds(1510);
}


void loop() 
{
String myT;
uint16_t mlx90640Frame[834];

    MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
    float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
    float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
    float tr = Ta - TA_SHIFT;
    float emissivity = 0.95;
    MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);

     
//spiWrite(REG3080_Frame_Period_Upper, 0x00);
//uint8_t FrameRateUB = spiRead(REG3080_Frame_Period_Upper);
//spiWrite(REG3080_Frame_Period_Lower, 0x00);
//uint8_t FrameRateLB = spiRead(REG3080_Frame_Period_Lower);


spiWrite(ADNS3080_FRAME_CAPTURE, 0x83); //capture frame 
delayMicroseconds(1510); //~3 frames
//tft.fillRect(65,0, 30,20, ST77XX_WHITE);

TIME0 = millis();
for (int i1 = 0 ; i1 < 30 ; i1++)
{
for (int i2 = 0 ; i2 < 30 ; i2++)
{
  
uint8_t regValue = spiRead(ADNS3080_FRAME_CAPTURE);
uint8_t pixelValue = regValue << 2; 
float mycol565 = (pixelValue==252?RGB565(0, 0, 255):RGB565(pixelValue, pixelValue, pixelValue));

float s=4;
tft.fillRect(30*s-i1*s,30*s-i2*s, s,s, mycol565);
}
}



for (int i1 = 0 ; i1 < 32 ; i1++)  {
   for (int i2 = 0 ; i2 < 24 ; i2++)  {
    float tT = mlx90640To[i1+i2*32];
    float mycolor;

    //get Iron Palette 
    mycolor = 0;//Black
    mycolor = (tT>=25?RGB565(0x8C, 0x00, 0x20):mycolor);//Blue
    mycolor = (tT>=30?RGB565(0x77, 0x00, 0xCC):mycolor);//Purple
    mycolor = (tT>=35?RGB565(0x00, 0xD7, 0xFF):mycolor);//Gold
    mycolor = (tT>=40?RGB565(0xFF, 0xFF, 0xFF):mycolor);//White
    s = 4;
    tft.fillRect(33*s+i2*s, i1*s, s,s, mycolor);
    if (i1+i2*32==768/2+15) {myT=String(tT,0); tft.fillRect(33*s+i2*s, i1*s, s,s, RGB565(0x00, 0x00, 0xff));} //Center
  }
 }

  tft.fillRect(1,160, 240,30, RGB565(0x88, 0x88, 0x88));
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_BLACK);
  tft.setCursor(2, 161);
  tft.print(myT+"C");



  TIME1 = millis();
  tft.fillRect(1,190, 240,30, ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_BLACK);
  tft.setCursor(2, 191);
  tft.print((TIME1)*0.001);  
  //tft.print(ShutterUB*255+ShutterLB); 
  //tft.print(24000000/(FrameRateLB+0xFF*FrameRateUB));  
   
tft.setCursor(150, 191);
//  tft.print(24000000/(0xFF+0xFF*0xFF));  
tft.print("sec");  
  delay(10);
    
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

