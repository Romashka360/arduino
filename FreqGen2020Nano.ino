#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <FrequencyTimer2.h>  //arduino.cc/playground/Code/FrequencyTimer2

byte TP = 0b10101010; // Every other port receives the inverted signal

Adafruit_PCD8544 display = Adafruit_PCD8544(/*SCLK=*/ 7, /*DIN=*/ 6, /*DC=*/ 5, /*CS=*/ 4, /*RST=*/ 2); //Nokia 5110

const int ledPin =  13;      
int buttonState1 = 0, buttonState2 = 0, buttonState3 = 0, buttonState4 = 0;   

void setup() 
{
 pinMode(9, INPUT_PULLUP);   // initialize the pushbutton pin as an input:
 pinMode(10, INPUT_PULLUP);    
 pinMode(8, INPUT_PULLUP);    
 pinMode(12, INPUT_PULLUP);   
 pinMode(11, OUTPUT);
 display.begin();
}


ISR(TIMER1_COMPA_vect) 
{
PORTC = TP; // Send the value of TP to the outputs
TP = ~TP; // Invert TP for the next run
}


int ff[8]={1,1,1,1,1,1,6,1};
int fpos = 8;
int tposa[8]={10,9,7,6,5,3,2,1};
int flag;

void loop() 
{
display.clearDisplay(); 
buttonState1 = digitalRead(12);
buttonState2 = digitalRead(8);
buttonState3 = digitalRead(10);
buttonState4 = digitalRead(9);

 if (buttonState1 == LOW)
 {   
 flag = 1;
 ff[fpos-1] = ff[fpos-1] - 1; 
 if (ff[fpos-1] < 1) ff[fpos-1] = 1;
 delay(150);
 } 

 if (buttonState2 == LOW)
 { 
 flag = 1;  
 ff[fpos-1] = ff[fpos-1] + 1; 
 if (ff[fpos-1] > 10) ff[fpos-1] = 10;
 delay(150);
 } 


 if (buttonState3 == LOW)
 {   
 fpos = fpos - 1; 
 if (fpos < 1) fpos = 1;
 delay(150);
 } 

 if (buttonState4 == LOW)
 {   
 fpos = fpos + 1; 
 if (fpos > 8) fpos = 8;
 delay(150);
 } 

 display.setTextSize(1);
 display.setTextColor(BLACK);
 display.setCursor(0,0);
 display.println("f:          Hz");


float myfreq = 0.0;
float k3 = 1.0;

for (int ii=1; ii<=8; ii+=1) 
 {
  display.setCursor(11+(10-tposa[ii-1])*6,0);
  display.println(ff[ii-1]-1);
  float f3 = ff[8-ii]-1.0;
  myfreq = myfreq + k3*f3; //1.0*ff2[7]+10.0*ff2[6]+100.0*ff2[5]+1000.0*ff2[4]+10000.0*ff2[3]+100000.0*ff2[2]+1000000.0*ff2[1]+10000000.0*ff2[0];
  k3 = k3*10.0;  
 }

float myk = 8000000.0/myfreq;
long mykk = myk;

 
int trh = 14;
int trp = 13+(10-tposa[fpos-1])*6;
display.fillTriangle(trp, trh-5, trp-3, trh, trp+3, trh, BLACK);

for (int16_t i=2; i<12; i+=1) 
 {
 display.fillRect(i*6-1, 16, 5, 2, BLACK);
 }


if (flag>0)
{
flag=0;

if ((myfreq <= 205000)&&(myfreq >=150))
{
DDRC = 0b11111111; // A0 from 150Hz to 205kHz
// Initialize Timer1
//noInterrupts(); // Disable interrupts
TCCR1A = 0;
TCCR1B = 0;
TCNT1 = 0;
OCR1A = mykk;
TCCR1B |= (1 << WGM12); // CTC mode
TCCR1B |= (1 << CS10); // Set prescaler to 1 ==> no prescaling
TIMSK1 |= (1 << OCIE1A); // Enable compare timer interrupt
//interrupts(); // Enable interrupts
}


if (myfreq <=1000000)
{
FrequencyTimer2::setPeriod(1000000.0/myfreq);
FrequencyTimer2::enable();  //D11 from 30Hz to 1MHz
}
}

if (myfreq >= 8000000)
{
PORTB = 0b00100000;  //D13 at 8MHz
PORTB = 0b00000000;  
PORTB = 0b00100000;  
PORTB = 0b00000000;  
PORTB = 0b00100000;  
PORTB = 0b00000000;  
PORTB = 0b00100000;  
PORTB = 0b00000000;  
}


 display.display();
 delay(150);
}
