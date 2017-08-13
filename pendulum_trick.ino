#include <Servo.h>
 
 
//pendulum all the way to the right (-) is 0 degrees
//pendulum all the way to the left (+) is 180 degrees
//pendulum straight down (N) is 90 degrees
//minusnow button to move the pendulum instantly to horizontal left
//plusnow button to move the pendulum instantly to the right
//startbutton: press button to stop the program and restart
 
int pstart=0; //pendulum start position - neutral or straight down
int servoDelay = 25; //
int inc = 10; //degrees to increment the servo - planned at 10 degrees
int pos = 0; //servo position and location of the pendulum
int startpend = 1; //flag to know striphard and no other sensor is monitored
int startbutton = 0;
int minusnowbutton = 0;

boolean startstop = false;
 
const int behindsensorPin = 4;     // behindsensor connected to pin 4
const int frontsensorPin = 5;       //frontsensor connected to pin 5
const int striphardsensorPin = 6;   //striphard sensor connecte to pin 6
const int minusnowbuttonPin = 7;   //striphard sensor connecte to pin 7
const int plusnowbuttonPin = 8;   //striphard sensor connecte to pin 8
const int startbuttonPin = 9;   //striphard sensor connecte to pin 9
const int servoPin = 3; //servo attached to pin 3 

Servo myPendulum; //myPendulum is the servo and pendulum name
 
 
 
void setup() {
myPendulum.attach(servoPin); 
pos=90; // set position variable to 90 degrees
myPendulum.write (pos); //put pendulum to 90 degrees - neutral
Serial.begin(9600);
 
pinMode(behindsensorPin, INPUT);
pinMode(frontsensorPin, INPUT);
pinMode(striphardsensorPin, INPUT);
pinMode(minusnowbuttonPin, INPUT);
pinMode(plusnowbuttonPin, INPUT);
pinMode(startbuttonPin, INPUT);
}
 
void loop() {
 
//scan for start button and if high then stop program and return p to 90 - startbutton
if(startbutton == HIGH) //Positive voltage to pin is HIGH
{
 pos=90; 
 myPendulum.write (pos);
 Serial.println("startbutton pressed, servo at 90");
 startstop=true;
}
 if(startstop)
 {
//scan for minusnow button and if high then p=180 - minusnow
 
if(minusnowbutton == HIGH)
{
  pos=180;
  myPendulum.write (pos);
  Serial.println("minusnow button pressed, servo at 180");
  startstop=false;
}
 
//scan for plus button and if high then p=0 - plusnow
if (plusnowbuttonPin == HIGH)
{
 pos=0; 
 myPendulum.write(pos);
 Serial.println("plusnow button pressed, servo at 0");
 startstop=false;
}
 
//start of the routine - must press striphard, and startpend =1 and pos=90
 
if (striphardsensorPin == HIGH && pos==90)
{
  pos=pos+inc;
  myPendulum.write (pos);
  Serial.println("striphardsenor detected, pendulum moves 10 degrees toward +");
  Serial.println(pos);
}
 
 
//if behindsensor is high, pos = pos-10
 
if (behindsensorPin == HIGH && pos!=90)
{
  pos=pos-inc;
  myPendulum.write (pos);
    Serial.println("behindsenor detected, pendulum moves 10 degrees toward +");
    Serial.println(pos);
}
 
//if frontsensor is high, pos = pos+10
 
if (frontsensorPin == HIGH && pos!=90)
{
  pos=pos+inc;
  myPendulum.write (pos);
    Serial.println("frontsenor detected, pendulum moves 10 degrees toward -");
    Serial.println(pos);
 
}
}
//scan for pos <=0 or >=180, if so then program complete and deactivate all buttons except startbutton
 
 
//add HALT code here
 
}
