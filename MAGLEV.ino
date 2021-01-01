int set_point = 100;  // adjust
int sensorPin = A0;
int output_pin = 3;
int sensorValue = 0;

void setup() {
  //Serial.begin(9600); //comment for stability
  pinMode(output_pin, OUTPUT);
}

void loop() 
{
      sensorValue = analogRead(sensorPin);
      //Serial.println(sensorValue); //comment for stability
      if( sensorValue <= set_point)
      digitalWrite(output_pin, LOW);
      else
      digitalWrite(output_pin, HIGH);
 }
