/*
 The circuit:
 * SD card attached to SPI bus as follows:
 * VCC - 5V
 * GND - GND
 * MOSI - pin 51
 * MISO - pin 50
 * SCK - pin 52
 * CS - pin 53
 
 * Body - A1
 * Ground - GND
 * Battery - 5V

 * Buzzer - pin 5
 * Red LED SD card indicator - pin 6 
 * Ampermeter+ - pin 7 
 * Ampermeter- - GND
*/

 
#include <SPI.h>
#include "SdFat.h"
SdFat SD;

#define SD_CS_PIN SS
File myFile;
unsigned long time;

void setup() {

 pinMode(7, OUTPUT); //Ampermeter
 pinMode(6, OUTPUT); //SD card indicator
  
  Serial.begin(9600);
  while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN)) { Serial.println("initialization failed!"); return; }
  Serial.println("initialization done.");
  
}

void loop() {

  int sensorValue = analogRead(A1);

  Serial.print(" Sensor: ");
  Serial.print(sensorValue);
    
  int Indicator1 = (255.0-sensorValue/4.0)*1.0;
  analogWrite(7, Indicator1*255.0/255.0); // Output to ampermeter
  //analogWrite(7, 255.0);
  tone(6, 90.0+1000*log10(abs(Indicator1))); // Output to buzzer
  String string1 = String(Indicator1/255.0, 3);
  time = millis();
  String string2 = String(time/1000.0,3);

    Serial.print(" Time: ");
    Serial.print(string2);
    Serial.print(" Indicator: ");
    Serial.print(string1);
    Serial.println(" ");
 
  myFile = SD.open("mytest.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(string2);
    myFile.print(" ");
    myFile.print(string1);
    myFile.println(" ");
    analogWrite(5, 255.0); // Output to LED
    delay(50); 
    analogWrite(5, 0.0);
    delay(50); 
  } else {
    Serial.println("error opening test.txt");
    analogWrite(5, 0.0);
  }

  myFile.close();

}
