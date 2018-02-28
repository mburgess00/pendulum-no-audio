#include <SoftwareSerial.h>
#include <Adafruit_Soundboard.h>

#include <Servo.h>
#include <IR_remote.h>
#include <EEPROM.h>

//how long to wait between moves
const long interval = 2000; //5s

//Initialize Sound Board
#define SFX_TX 5
#define SFX_RX 6
#define SFX_RST 4

SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);


//Initialize Sensor Pins

#define SENS45 7
#define SENS55 8
#define SENS65 9
#define SENS75 10
#define SENS85 11

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

//int RECV_PIN = 2;
int RECV_PIN = 14;
IRrecv irrecv(RECV_PIN);
decode_results results;
uint16_t lastCode = 0; // This keeps track of the last code RX'd


//Initialize Servo
int SERVO_PIN = 3;
Servo myservo;
int pos;

int count = 0;

long lastmove;

int eeAddress = 0;
int calibration;

int posnum = 6;

int program = 3;
int prevprogram = 3;
//tracks:
//  0 = default
//  1 = construction
//  2 = existing
//  3 = financial
//  4 = retail
//  5 = technology
const char filenames[][6] = 
{
  "DEFAU",
  "CONST",
  "EXIST",
  "FINAN",
  "RETAI",
  "TECHN"
};
int track = 0;

long lastmillis;

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

  pinMode(SENS45, INPUT);
  pinMode(SENS55, INPUT);
  pinMode(SENS65, INPUT);
  pinMode(SENS75, INPUT);
  pinMode(SENS85, INPUT);


  //servo
  myservo.attach(SERVO_PIN);

  ss.begin(9600);
 
  if (!sfx.reset()) {
    Serial.println("Not found");
    while (1);
  }
  Serial.println("SFX board found");

  uint8_t files = sfx.listFiles();

  Serial.println("File Listing");
  Serial.println("========================");
  Serial.println();
  Serial.print("Found "); Serial.print(files); Serial.println(" Files");
  for (uint8_t f=0; f<files; f++) {
    Serial.print(f); 
    Serial.print("\tname: "); Serial.print(sfx.fileName(f));
    Serial.print("\tsize: "); Serial.println(sfx.fileSize(f));
  }
  Serial.println("========================");

    
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
  lastmillis = millis();

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

void moveServoByNum(int position)
{
  switch (position)
  {
    case 3:
    {
      pos = calibration - 90;
    }
    break;
    case 4:
    {
      pos = calibration - 60;
    }
    break;
    case 5:
    {
      pos = calibration - 30;
    }
    case 6:
    {
      pos = calibration;
    }
    case 7:
    {
      pos = calibration + 30;
    }
    case 8:
    {
      pos = calibration + 60;
    }
    case 9:
    {
      pos = calibration + 90;
    }
    break;
  }
  moveServo(pos);
}

void loop() {

  char trackname[20];

  if ((millis() - interval) > lastmillis)
  {
    lastmillis = millis();
    Serial.print("Track: ");
    Serial.println(track);
    Serial.print("Program mode: ");
    Serial.println(program);
  }

  if (program == 1)
  {
    switch (posnum)
    {
      case 3:
        //position 3
	//do nothing - you win!
      break;
      case 4:
        //position 4
	//see if any targets hit
	//if 4.5 hit, move to 3
	//otherwise move to 5
	if (digitalRead(SENS45))
	{
	  posnum = 3;
	  //move servo
	  moveServoByNum(posnum);
	  //play T03 sound
          sprintf(trackname, "%s%s%s", "T03", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS55) || digitalRead(SENS65) || digitalRead(SENS75) || digitalRead(SENS85))
	{
	  posnum = 5;
	  //move servo
	  moveServoByNum(posnum);
	  //play T05 sound
          sprintf(trackname, "%s%s%s", "T05", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 5:
        //position 5
	//see if any targets hit
	//if 5.5 hit, move to 4
	//otherwise move to 6
	if (digitalRead(SENS55))
	{
	  posnum = 4;
	  //move servo
	  moveServoByNum(posnum);
	  //play T04 sound
          sprintf(trackname, "%s%s%s", "T04", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS45) || digitalRead(SENS65) || digitalRead(SENS75) || digitalRead(SENS85))
	{
	  posnum = 6;
	  //move servo
	  moveServoByNum(posnum);
	  //play T06 sound
          sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 6:
        //position 6
	//see if any targets hit
	//if 7.5 hit, move to 5
	//otherwise move to 7
	if (digitalRead(SENS75))
	{
	  posnum = 5;
	  //move servo
	  moveServoByNum(posnum);
	  //play T05 sound
          sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS45) || digitalRead(SENS55) || digitalRead(SENS65) || digitalRead(SENS85))
	{
	  posnum = 7;
	  //move servo
	  moveServoByNum(posnum);
	  //play T07 sound
          sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 7:
        //position 7
	//see if any targets hit
	//if 7.5 hit, move to 6
	//otherwise move to 8
	if (digitalRead(SENS75))
	{
	  posnum = 6;
	  //move servo
	  moveServoByNum(posnum);
	  //play T06 sound
          sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS45) || digitalRead(SENS55) || digitalRead(SENS65) || digitalRead(SENS85))
	{
	  posnum = 8;
	  //move servo
	  moveServoByNum(posnum);
	  //play T08 sound
          sprintf(trackname, "%s%s%s", "T08", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 8:
        //position 8
	//see if any targets hit
	//if 8.5 hit, move to 7
	//otherwise move to 9
	if (digitalRead(SENS85))
	{
	  posnum = 7;
	  //move servo
	  moveServoByNum(posnum);
	  //play T07 sound
          sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS45) || digitalRead(SENS55) || digitalRead(SENS65) || digitalRead(SENS75))
	{
	  posnum = 9;
	  //move servo
	  moveServoByNum(posnum);
	  //play T09 sound
          sprintf(trackname, "%s%s%s", "T09", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 9:
        //position 9
	//do nothing - you lose :(
      break;
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
          //select which program
          if (program == 0)
          {
            //exit calibration mode
            Serial.println("Calibration mode disabled");
            EEPROM.put(eeAddress, pos);
            calibration = pos;
            program = prevprogram;
          }
          else
          {
            Serial.println("Mode select enabled");
            //call audio file "please select program"
            prevprogram = program;
            program = 4;
          }
        }
        if (count == 15)
        {
          program = 0;
          Serial.println("Calibration mode enabled");
          //play audio for calibration mode
        }
        break;
      case BUTTON_A:
        Serial.println("A");
        switch (program)
        {
          case 0: //calibration
            break;
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to far left
            pos = 180;
            moveServo(pos);
            break;
          case 4: //program select mode
            program = 1;
	    pos = calibration;
	    posnum = 6;
	    moveServoByNum(posnum);
	    //play T06 sound
            sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            break;
        }
        //Serial.println(pos);
	//moveServo(pos);
        break;
      case BUTTON_B:
        Serial.println("B");
        switch (program)
        {
          case 0: //calibration
            break;
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to bottom (cal)
            pos = calibration;
            moveServo(pos);
            break;
          case 4: //program select mode
            program = 2;
            break;
        }
        break;
      case BUTTON_C:
        Serial.println("C");
        switch (program)
        {
          case 0: //calibration
            break;
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to far right
            pos = 0;
            moveServo(pos);
            break;
          case 4: //program select mode
            program = 3;
            break;
        }
        break;
      case BUTTON_UP:
        Serial.println("Up");
        switch (program)
        {
          case 0: //calibration
            break;
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            break;
          case 4: //program select mode
            if ((track < 5)  && (count == 0))
            {
              track++;
              sprintf(trackname, "%s%s%s", "T00", filenames[track], "OGG\n");
	      Serial.println(trackname);
              sfx.playTrack(trackname);
            }
            break;
        }
        break;
      case BUTTON_DOWN:
        Serial.println("Down");
        switch (program)
        {
          case 0: //calibration
            break;
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            break;
          case 4: //program select mode
	    if ((track > 0) && (count == 0))
            {
              track--;
              sprintf(trackname, "%s%s%s", "T00", filenames[track], "OGG\n");
	      Serial.println(trackname);
              sfx.playTrack(trackname);
            }
            break;
        }
        break;
      case BUTTON_LEFT:
        Serial.println("Left");
        switch (program)
        {
          case 0: //calibration
            if (pos < 180)
            {
              pos++;
            }
            break;
          case 1: //program A - interact with pendulum
            //move pendulum to left
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to left
            if (pos < 180)
            {
              pos += 10;
              moveServo(pos);
            }
            break;
          case 4: //program select mode
            break;
        }
        break;
      case BUTTON_RIGHT:
        Serial.println("Right");
        switch (program)
        {
          case 0: //calibration
            if (pos > 0)
            {
              pos--;
            }
            break;
          case 1: //program A - interact with pendulum
            //move pendulum to right
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to right
            if (pos > 0)
            {
              pos -= 10;
              moveServo(pos);
            }
            break;
          case 4: //program select mode
            break;
        }
        break;
      case BUTTON_CIRCLE:
        Serial.println("Circle");
        switch (program)
        {
          case 0: //calibration
            break;
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            //see if pendulum is in the "correct" position
            break;
          case 3: //program C - instructor mode
            break;
          case 4: //program select mode
            break;
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
