#include <Adafruit_SSD1306.h>
#include "arduinoFFT.h" //github.com/kosme/arduinoFFT

arduinoFFT FFT = arduinoFFT();

const uint16_t samples = 128; //This value MUST ALWAYS be a power of 2
double vReal[samples];
double vImag[samples];

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
}

void loop()
{
  int scale = analogRead(A2);
  double samplingFrequency = 100*(scale/100)+10;
  double dt_us = 1000000/samplingFrequency;
  unsigned long tb = micros();
  int i;

  for(i=0; i<samples; i++)
  {
  vReal[i] = analogRead(A1);
  vImag[i] = 0;
  while(micros() - tb < dt_us) {}
  tb += dt_us;
  }

  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, samples);
  double P = FFT.MajorPeak(vReal, samples, samplingFrequency);

  float vm=0;

  for (i=2; i<samples/2; i++)
  {
  vm = vm<vReal[i] ? vReal[i] : vm;
  }

  for (i=2; i<samples/2; i++)
  {
  display.fillRect(i, 32-vReal[i]/vm*32, 1, vReal[i]/vm*32+1, WHITE);
  }


  display.setTextColor(WHITE);
  display.setCursor(60,1);
  display.print("Peak:");
  int precision = P<10 ? 1 : 0;
  display.print(P,precision);
  display.println("Hz");

  display.setCursor(60,10);
  display.print("0..");
  display.print(samplingFrequency/2,0);
  display.println("Hz");

  display.display();
  display.clearDisplay();
  }
