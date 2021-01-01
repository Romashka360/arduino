#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h> //github.com/adafruit/DHT-sensor-library
#include <DHT_U.h>

#define BME_scl 12
#define BME_sdo 9
#define BME_sda 11
#define BME_csb 10
Adafruit_BME280 myBME(BME_csb, BME_sda, BME_sdo, BME_scl);

#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22    
DHT_Unified myDHT(DHTPIN, DHTTYPE);


void setup() 
{
    myBME.begin();  
}


float runT[30]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float pU[30]={5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
int ii = 0;
 
void loop() 
{
  float Tem, Hum, Pre;
  float temperature, humidity;
  float celsius, fahrenheit;
  float pinf, pin6, pin2, pinU;
  sensors_event_t event0, event1;  

  pinU=0;
  pU[ii] = readVcc()*0.001;
  pin2 = 0;
  runT[ii] = analogRead(A2);
  ii = (ii+1) % 30; 
  for (int i=0;i<30;i++)
  {
    pin2 = pin2 + runT[i]/30.0;
    pinU = pinU + pU[i]/30.0;
  }

  celsius = (pin2/1023.0*pinU-0.5)*100.0;
  fahrenheit = celsius*1.8+32.0;
   
  Tem = myBME.readTemperature();
  Pre = myBME.readPressure()/101325.0144354*760.0;
  Hum = myBME.readHumidity();

  myDHT.temperature().getEvent(&event0);
  myDHT.humidity().getEvent(&event1);
  temperature = event0.temperature == 0 ? 0.0 : event0.temperature;
  humidity = event1.relative_humidity == 0 ? 0.0 : event1.relative_humidity;


pin6 = analogRead(A6);
//resistors R1&R4:pin6 = 344
//resistors R1&(R3+R4):pin6 = 614
//resistors R1&R3:pin6 = 510

if (pin6 > 300) {
  pinf = (event1.relative_humidity - 0.0)/100.0;
  }

if (pin6 > 400) {
  pinf = (Pre - 690.0)/(790.0-690.0);
  }

if (pin6 > 600) {
  pinf = (celsius - (-20.0))/(80.0-(-20.0));
  }

analogWrite(6, 5.0/pinU*pinf*255.0/1.82); //resistor R2
delay(200);
}


long readVcc() {
ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  // Read 1.1V reference against AVcc
delay(2);
ADCSRA |= _BV(ADSC);
while (bit_is_set(ADCSRA,ADSC));
long result = ADCL;
result |= ADCH<<8;
return 1125300L  / result;
}
