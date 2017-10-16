#include <Servo.h>
#include <IRremote.h>

/*pins
#PIN2 = IR DATA
#PIN3 = Servo
#PIN4 = Left ultrasonic trigger
#PIN5 = Left ultrasonic echo
#PIN7 = Left ultrasonic trigger
#PIN6 = Left ultrasonic echo
*/

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
int pos = 90;

void setup()
{
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");

  //servo
  myservo.attach(SERVO_PIN);
  myservo.write(pos);
}

void loop() {
  if (irrecv.decode(&results)) 
  {
    /* read the RX'd IR into a 16-bit variable: */
    uint16_t resultCode = (results.value & 0xFFFF);

    /* The remote will continue to spit out 0xFFFFFFFF if a 
     button is held down. If we get 0xFFFFFFF, let's just
     assume the previously pressed button is being held down */
    if (resultCode == 0xFFFF)
      resultCode = lastCode;
    else
      lastCode = resultCode;

    // This switch statement checks the received IR code against
    // all of the known codes. Each button press produces a 
    // serial output, and has an effect on the LED output.
    switch (resultCode)
    {
      case BUTTON_POWER:
        Serial.println("Power");
        break;
      case BUTTON_A:
        Serial.println("A");
        break;
      case BUTTON_B:
        Serial.println("B");
        break;
      case BUTTON_C:
        Serial.println("C");
        break;
      case BUTTON_UP:
        Serial.println("Up");
        break;
      case BUTTON_DOWN:
        Serial.println("Down");
        break;
      case BUTTON_LEFT:
        Serial.println("Left");
        if (pos > 10)
        {
          pos -= 10;
          Serial.println(pos);
          myservo.write(pos);
        }
        break;
      case BUTTON_RIGHT:
        Serial.println("Right");
        if (pos < 170)
        {
          pos += 10;
          Serial.println(pos);
          myservo.write(pos);
        }
        break;
      case BUTTON_CIRCLE:
        Serial.println("Circle");
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
