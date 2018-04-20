// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 6, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ENC_SW A2
#define ENC_B A0
#define ENC_A A1
#define ENC_CLOCKWISE 1
#define ENC_ANTI_CLOCKWISE 2
#define ENC_BUTTON 3
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
  int swVal = digitalRead(ENC_SW);
  if (swVal == LOW) {
    unsigned long thisActivation = millis();
    if(thisActivation - lastActivation > 1000) {
      lastActivation = thisActivation;
      return ENC_BUTTON;
    }
  }
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
  //lcd.print("hello, world!");

  // Setup Temp
  sensors.begin();
}

int fanSpeed = 0;
int lastFanSpeed = -1;
int servoPos = 0;
int lastServoPos = -1;
int backlightPercent = 100;
int lastBacklightPercent = -1;
int mode = 0;
int lastMode = -1;
float temp = 0;
float lastTemp = -1;
unsigned long lastSensorPollTime = -1000;

void drawScreen() {
  if(mode != lastMode) {
    lastMode = mode;
    lcd.clear();
    if(mode == 0) {
      lcd.print("Set FAN");
    }
    else if(mode == 1) {
      lcd.print("Set Servo");
    }
    else if(mode == 2) {
      lcd.print("Set Backlight");
    }
  }
  lcd.setCursor(0, 1);
  if(mode == 0) {
    lcd.print(fanSpeed);
  }
  else if(mode == 1) {
    lcd.print(servoPos);
  }
  else if(mode == 2) {
    lcd.print(backlightPercent);
  }
  lcd.print("  ");
  //if(lastTemp != temp) {
    lastTemp = temp;
    lcd.setCursor(6, 1);
    lcd.print(temp);
  //}
}

void updateTemps() {
  unsigned long now = millis();
  if (now - lastSensorPollTime >= 5000) {
    lastSensorPollTime = now;
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int rotaryEncoderRes = readEncoder();
  if(rotaryEncoderRes == ENC_CLOCKWISE) {
    if(mode == 0)
      fanSpeed++;
    if(mode == 1)
      servoPos++;
  }
  if(rotaryEncoderRes == ENC_ANTI_CLOCKWISE) {
    if(mode == 0)
      fanSpeed--;
    if(mode == 1)
      servoPos--;
  }
  if(rotaryEncoderRes == ENC_BUTTON) {
    mode++;
    if(mode > 2)
      mode = 0;
  }
  if(fanSpeed != lastFanSpeed) {
    if(fanSpeed > 100)
      fanSpeed = 0;
    if(fanSpeed < 0)
      fanSpeed = 100;
    lastFanSpeed = fanSpeed;
    OCR2B = map(fanSpeed, 0, 100, 0, 79);

  }
  if(servoPos != lastServoPos) {
    if(servoPos > 180)
      servoPos = 0;
    if(servoPos < 0)
      servoPos = 180;
    lastServoPos = servoPos;
    servo.write(servoPos);
  }
  updateTemps();
  drawScreen();
  //delay(200);
  //fanSpeed++;
}
