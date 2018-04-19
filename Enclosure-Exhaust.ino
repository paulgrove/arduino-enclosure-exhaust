// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 6, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ENC_SW A2
#define ENC_B A0
#define ENC_A A1
int pinALast;
int returnLast;
int aVal;
unsigned long lastActivation = millis();

Servo servo;

void setupEncoder() {
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(ENC_SW, INPUT);
  pinALast = digitalRead(ENC_A);
}

int readEncoder() {
  boolean bCW;
  int returnVal = 0;
  aVal = digitalRead(ENC_A);
  if (aVal != pinALast) {
    unsigned long thisActivation = millis();
    if(thisActivation - lastActivation > 100) {
      lastActivation = thisActivation;
      // knob is rotating - get direction
      if (digitalRead(ENC_B) != aVal) {
        bCW = true;
        returnVal = 1;
      } else {
        bCW = false;
        lastActivation = millis();
        returnVal = 2;
      }
    }
  }
  pinALast = aVal;
  if(returnLast == returnVal) {
    return 0;
  }
  returnLast = returnVal;
  return returnVal;
}

void setup() {
  servo.attach(9, 610, 2395);
  /***********************
   * Fan PWM setup on D3
   */
  pinMode(3, OUTPUT);
  // Fast PWM Mode, Prescaler = /8
  // PWM on Pin 3, Pin 11 disabled
  // 16Mhz / 8 / (79 + 1) = 25Khz
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS21);
  // Set TOP and initialize duty cycle to zero(0)
  OCR2A = 79;   // TOP - DO NOT CHANGE, SETS PWM PULSE RATE
  OCR2B = 0;    // duty cycle for Pin 3 (0-79) generates 1 500nS pulse even when 0
  
  /***********************
   * LCD Setup
   */
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

int fanSpeed = 0;
int lastFanSpeed = -1;
void loop() {
  // put your main code here, to run repeatedly:
  OCR2B = map(fanSpeed, 0, 100, 0, 79);
  int rotaryEncoderRes = readEncoder();
  if(rotaryEncoderRes == 1)
    fanSpeed++;
  if(rotaryEncoderRes == 2)
    fanSpeed--;
  if(fanSpeed != lastFanSpeed) {
    lastFanSpeed = fanSpeed;
    lcd.setCursor(0, 1);
    lcd.print(fanSpeed);
    OCR2B = map(fanSpeed, 0, 180, 0, 79);
    servo.write(fanSpeed);
  }
  //delay(200);
  //fanSpeed++;
  if(fanSpeed > 180)
    fanSpeed = 0;
  if(fanSpeed < 0)
    fanSpeed = 180;
}
