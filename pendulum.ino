#include <Servo.h>
#include <IR_remote.h>
#include <EEPROM.h>

/*pins
#PIN2 = IR DATA
#PIN3 = Servo
#PIN4 = Left ultrasonic trigger
#PIN5 = Left ultrasonic echo
#PIN7 = Right ultrasonic trigger
#PIN6 = Right ultrasonic echo
*/

//how long to wait between moves
const long interval = 2000; //5s

//ultrasonic
#define trigPinL 4
#define trigPinR 7
#define echoPinL 5
#define echoPinR 6

//Initialize Remote
const uint16_t BUTTON_POWER = 0xD827; // i.e. 0x10EFD827
const uint16_t BUTTON_A = 0xF807;
const uint16_t BUTTON_B = 0x7887;
const uint16_t BUTTON_C = 0x58A7;
const uint16_t BUTTON_UP = 0xA05F;
const uint16_t BUTTON_DOWN = 0x00FF;
const uint16_t BUTTON_LEFT = 0x10EF;
const uint16_t BUTTON_RIGHT = 0x807F;
const uint16_t BUTTON_CIRCLE = 0x20DF;

int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;
uint16_t lastCode = 0; // This keeps track of the last code RX'd


//Initialize Servo
int SERVO_PIN = 3;
Servo myservo;
int pos;

boolean calmode=false;
boolean paused=true;
int count = 0;

long lastmove;

int eeAddress = 0;
int calibration;

void setup()
{
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.

  //wait on serial
  delay(5000);
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");

  //servo
  myservo.attach(SERVO_PIN);
 

  pinMode(trigPinL, OUTPUT);
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinL, INPUT);
  pinMode(echoPinR, INPUT);


  EEPROM.get( eeAddress, calibration);
  Serial.print("Calibration value");
  Serial.println(calibration);

  if (calibration != -1)
  {
    pos = calibration;
  }
  else
  {
    calibration = 90;
    pos = 90;
  }

  //myservo.write(pos);
  moveServo(pos);

  lastmove = millis();

}

long SonarSensor(int trigPin,int echoPin)
{
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  return distance;
}

void moveServo(int target)
{
  int current = myservo.read();
  int movepos;
//  Serial.print("Current servo pos: ");
//  Serial.println(current);
//  Serial.print("Moving to: ");
//  Serial.println(target);
  if (current < target)
  {
    for (movepos = current; movepos <= target; movepos += 1)
    {
//      Serial.print("Moving to: ");
//      Serial.println(movepos);
      myservo.write(movepos);
      delay(30);
    }
  }
  else if (current > target)
  {
    for (movepos =current; movepos >= target; movepos -= 1)
    {
//      Serial.print("Moving to: ");
//      Serial.println(movepos);
      myservo.write(movepos);
      delay(30);
    }
  }
}

void loop() {


  //read the ultrasonic sensors
  long LeftDistance, RightDistance;

  
  RightDistance = SonarSensor(trigPinR, echoPinR);
  delay(100);
  LeftDistance = SonarSensor(trigPinL, echoPinL);


  if (LeftDistance < 10)
  {
    Serial.print("Sensed Left ");
    Serial.println(LeftDistance);
  }
  if (RightDistance < 10)
  {
    Serial.print("Sensed Right ");
    Serial.println(RightDistance);
  }

  if ((!calmode) && ((millis() - interval) > lastmove) && (!paused))
  {
    //netural
    if (pos == calibration)
    {
      if ((LeftDistance < 10) && (LeftDistance > 5))
      {
        //move forward 10*
        pos = pos - 10;
      }
      if (RightDistance < 10)
      {
        pos = pos + RightDistance;
      }
    }
    else
    {
      if (LeftDistance < 5)
      {
        //move forward 10* if possible, else move to end
        if (pos > 9)
        {
          pos = pos - 10;
        }
        else
        {
          pos = 0;
        }
      }
      if (RightDistance < 10)
      {
        if (pos+RightDistance > 180)
        {
          pos = 180;
        }
        else
        {
          pos = pos + RightDistance;
        }
      }
       
    }

//    Serial.println(pos);
    //myservo.write(pos);
    Serial.print("Current position: ");
    Serial.print(myservo.read());
    Serial.print(", Want to move to: ");
    Serial.println(pos);
    if (myservo.read() != pos)
    {
      moveServo(pos); 
      lastmove = millis();
    }
  }

  
  if (irrecv.decode(&results)) 
  {
    /* read the RX'd IR into a 16-bit variable: */
    uint16_t resultCode = (results.value & 0xFFFF);

    /* The remote will continue to spit out 0xFFFFFFFF if a 
     button is held down. If we get 0xFFFFFFF, let's just
     assume the previously pressed button is being held down */

    if (resultCode == 0xFFFF)
    {
      resultCode = lastCode;
      count++;
      Serial.println(count);
    }
    else
    {
      lastCode = resultCode;
      count = 0;
    }


    // This switch statement checks the received IR code against
    // all of the known codes. Each button press produces a 
    // serial output, and has an effect on the LED output.
    switch (resultCode)
    {
      case BUTTON_POWER:
        Serial.println("Power");
        if (count == 0)
        {
          paused = !paused;
          if (paused)
          {
            Serial.println("Pausing the ultrasonic sensors");
          }
          else
          {
            Serial.println("Resuming the ultrasonic sensors");
          }
        }
        break;
      case BUTTON_A:
        Serial.println("A");
        if (!calmode)
        {
          pos = 180;
          Serial.println(pos);
          //myservo.write(pos);
	  moveServo(pos);
        }
        break;
      case BUTTON_B:
        Serial.println("B");
        if (!calmode)
        {
          pos = calibration;
          Serial.println(pos);
          //myservo.write(pos);
	  moveServo(pos);
        }
        break;
      case BUTTON_C:
        Serial.println("C");
        if (!calmode)
        {
          pos = 0;
          Serial.println(pos);
          //myservo.write(pos);
	  moveServo(pos);
        }
        break;
      case BUTTON_UP:
        Serial.println("Up");
        break;
      case BUTTON_DOWN:
        Serial.println("Down");
        break;
      case BUTTON_LEFT:
        Serial.println("Left");
        if (calmode)
        {
          if (pos < 180)
          {
            pos++;
          }
        }
        else
        {
          if (pos < 171)
          {
            pos += 10;
          }
        }
        Serial.println(pos);
        //myservo.write(pos);
	moveServo(pos);
        break;
      case BUTTON_RIGHT:
        Serial.println("Right");
        if (calmode)
        {
          if (pos > 0)
          {
            pos--;
          }
        }
        else
        {
          if (pos > 9)
          {
            pos -= 10;
          }
        }
        Serial.println(pos);
        //myservo.write(pos);
	moveServo(pos);
        break;
      case BUTTON_CIRCLE:
        Serial.println("Circle");
        if (!calmode)
        {
          pos = calibration;
          //myservo.write(pos);
	  moveServo(pos);
        }
        
        if (count == 15)
        {
          calmode=!calmode;
          if (calmode)
          {
            Serial.println("Calibration mode enabled");
          }
          else
          {
            Serial.println("Calibration mode disabled");
            EEPROM.put(eeAddress, pos);
            calibration = pos;
          }
        }
        break;
      default:
        Serial.print("Unrecognized code received: 0x");
        Serial.println(results.value, HEX);
        break;        
    }    
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}
