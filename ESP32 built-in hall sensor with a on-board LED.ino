/const int ledPin2 = 2;  //Espduino
//const int ledPin2 = 2;  //NodeMCU
//const int ledPin2 = 25;  //Heltec
//const int ledPin2 = 4;  //ESP32-CAM bigLED
const int ledPin2 = 33;  //ESP32-CAM smallLED
//const int ledPin2 = ??;  //TTGO

int val = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(ledPin2, OUTPUT);
}

void loop()
{
  val = hallRead();
  Serial.println(val);
  digitalWrite(ledPin2, 0);
  delay(400+4.0*val); 
  digitalWrite(ledPin2, 1);
  delay(10); 
}
