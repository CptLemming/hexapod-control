// include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <PS2X_lib.h>  //for v1.6

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 52, en = 50, d4 = 47, d5 = 45, d6 = 43, d7 = 41;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const unsigned int TRIG_PIN=4;
const unsigned int ECHO_PIN=3;
#define cSSC_BAUD   9600
#define cSSC_OUT       12   //Output pin for Botboard - Input of SSC32 (Yellow)
#define cSSC_IN        13

PS2X ps2x;
int error = 0; 
byte type = 0;
byte vibrate = 0;
int mode = 0;
int leg = 0;
int joint = 0;
const int modeCount = 2;
const int legCount = 24;
const int jointCount = 4;

SoftwareSerial SSCSerial(cSSC_IN, cSSC_OUT);

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(57600);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  error = ps2x.config_gamepad(9,7,6,8, true, true);

  if(error == 0)
  {
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
  // Serial.print(ps2x.Analog(1), HEX);
  type = ps2x.readType();
  switch(type)
  {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  }
  // Print a message to the LCD.
  lcd.print("Starting!");

  SSCSerial.begin(cSSC_BAUD);

  // Reset all legs
  for (int i = 0; i < legCount; i++) {
    SSCSerial.print("#");
    SSCSerial.print(i, DEC);
    SSCSerial.print("P");
    SSCSerial.print(1500, DEC);
  }
  SSCSerial.print("T");
  SSCSerial.println(500, DEC);
  
}

void loop() {
//  digitalWrite(TRIG_PIN, LOW);
//  delayMicroseconds(2);
//  digitalWrite(TRIG_PIN, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(TRIG_PIN, LOW);
//
//  const unsigned long duration = pulseIn(ECHO_PIN, HIGH);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Mode: "+ String(mode));

  lcd.setCursor(0, 1);

  ps2x.read_gamepad(false, 0);

  if (ps2x.ButtonPressed(PSB_START)) {
    if (mode < (modeCount - 1)) {
      mode++;
    } else {
      mode = 0;
    }
  }

  if (mode == 0) {
    if (ps2x.ButtonPressed(PSB_SELECT)) {
      if (leg < (legCount - 1)) {
        leg++;
      } else {
        leg = 0;
      }
    }

    long pos = map(ps2x.Analog(PSS_LY), 0, 255, 750, 2250);
  
    lcd.print("Leg:"+ String(leg) + " P:"+ String(pos));
  
    SSCSerial.print("#");
    SSCSerial.print(leg, DEC);
    SSCSerial.print("P");
    SSCSerial.print(pos, DEC);
    SSCSerial.print("T");
    SSCSerial.println(500, DEC);
  } else if (mode == 1) {
    if (ps2x.ButtonPressed(PSB_SELECT)) {
      if (joint < (jointCount - 1)) {
        joint++;
      } else {
        joint = 0;
      }
    }

    long pos = map(ps2x.Analog(PSS_LY), 0, 255, 750, 2250);
    lcd.print("Joint:"+ String(joint) + " P:"+ String(pos));

    for (int i = 0; i < legCount; i++) {
      if (i % 4 == joint) {
        SSCSerial.print("#");
        SSCSerial.print(i, DEC);
        SSCSerial.print("P");
        SSCSerial.print(pos, DEC);
      }
    }
    SSCSerial.print("T");
    SSCSerial.println(500, DEC);
  }

//  lcd.setCursor(0, 1);
//  int distance= duration/29/2;
//  if (duration==0){
//    lcd.print("No pulse");
//  } else{
//    lcd.print(String(distance) + " cm");
//  }
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
//  lcd.setCursor(0, 1);
//  // print the number of seconds since reset:
//  lcd.print(millis() / 1000);

//  SSCSerial.print("#");
//  SSCSerial.print(16, DEC);
//  SSCSerial.print("P");
//  SSCSerial.print(750, DEC);
//  SSCSerial.print("T");
//  SSCSerial.println(1500, DEC);
//  delay(3000);                                                                     //Wait 1 second
//
//  SSCSerial.print("#");
//  SSCSerial.print(16, DEC);
//  SSCSerial.print("P");
//  SSCSerial.print(2250, DEC);
//  SSCSerial.print("T");
//  SSCSerial.println(1500, DEC);
//  delay(3000);

  delay(100);
}

