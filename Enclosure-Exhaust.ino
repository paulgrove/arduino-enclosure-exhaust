// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "rotaryEncoder.h"
#include "Fan4Pin.h"

#define ONE_WIRE_BUS 10

Fan4Pin fan;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 6, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RotaryEncoder rEncoder(A1, A0, A2);

Servo servo;

void setup() {
	servo.attach(9, 610, 2395);

	/***********************
	 * LCD Setup
	 */
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	// Print a message to the LCD.
	//lcd.print("hello, world!");

	// Setup Temp
	sensors.begin();

	fan.start();
	rEncoder.start();
}

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
		lcd.print(fan.getSpeed());
	}
	else if(mode == 1) {
		lcd.print(servoPos);
	}
	else if(mode == 2) {
		lcd.print(backlightPercent);
	}
	lcd.print("	");
	//if(lastTemp != temp) {
		lastTemp = temp;
		lcd.setCursor(6, 1);
		lcd.print(temp);
	//}
}

void updateTemps() {
	unsigned long now = millis();
	if (now - lastSensorPollTime >= 5000) {
		servo.detach();
		lastSensorPollTime = now;
		sensors.requestTemperatures();
		temp = sensors.getTempCByIndex(0);
		servo.attach(9, 610, 2395);
	}
}

void loop() {
	// put your main code here, to run repeatedly:
	RotaryEncoderState reState = rEncoder.read();
	if(reState == RotaryEncoderState::clockwise) {
		if(mode == 0)
			fan.setSpeed(fan.getSpeed() + 1);
		if(mode == 1)
			servoPos++;
	}
	if(reState == RotaryEncoderState::anti_clockwise) {
		if(mode == 0)
			fan.setSpeed(fan.getSpeed() - 1);
		if(mode == 1)
			servoPos--;
	}
	if(reState == RotaryEncoderState::button) {
		mode++;
		if(mode > 2)
			mode = 0;
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
}
