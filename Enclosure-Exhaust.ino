// include the library code:
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "rotaryEncoder.h"
#include "Fan4Pin.h"
#include "ServoController.h"
#include "DController.h"

#define ONE_WIRE_BUS 10

Fan4Pin fan;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 6, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RotaryEncoder rEncoder(A1, A0, A2);

ServoController servo(8, 610, 2395);

// sampleRate, tRangeLow, tRangeHigh
DController tempController(15000, 240000, 300000);

void setup() {
	Serial.begin(9600);
	servo.start();

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

bool draw = true;
int mode = 0;
int lastMode = -1;
float temp = 0;
float lastTemp = -1;
unsigned long lastSensorPollTime = -1000;
int coolingFactor = 5;
int lastCoolingFactor = -1;
int targetTemp = 35;
int lastTargetTemp = -1;
int ctrlChange = 0;

void drawScreen() {
	if(!draw)
		return;
	draw = false;
	lcd.clear();
	lcd.setCursor(0, 0);
	if(mode == 0) {
		lcd.print("Target Temp");
	}
	else if(mode == 1) {
		lcd.print("Cooling Factor");
	}
	lcd.setCursor(0, 1);
	lcd.print(targetTemp);
	lcd.setCursor(4, 1);
	lcd.print(coolingFactor);
	lcd.setCursor(7, 1);
	lcd.print(temp);
}

void updateTemps() {
	unsigned long now = millis();
	if (now - lastSensorPollTime >= 5000) {
		lastSensorPollTime = now;
		sensors.requestTemperatures();
		temp = sensors.getTempCByIndex(0);
		if(temp != lastTemp) {
			lastTemp = temp;
			draw = true;
		}
	}
}

void loop() {
	// put your main code here, to run repeatedly:
	RotaryEncoderState reState = rEncoder.read();
	if(mode == 0) {
		if(reState == RotaryEncoderState::clockwise) {
			targetTemp++;
			draw = true;
		}
		else if (reState == RotaryEncoderState::anti_clockwise) {
			targetTemp--;
			draw = true;
		}
		if (targetTemp > 60)
			targetTemp = 60;
		else if (targetTemp < 0)
			targetTemp = 0;
		ctrlChange = tempController.update(temp, targetTemp);
		coolingFactor += ctrlChange;
		if (coolingFactor > 20)
			coolingFactor = 20;
		else if (coolingFactor < 0)
			coolingFactor = 0;
	}
	else if (mode == 1) {
		if(reState == RotaryEncoderState::clockwise) {
			coolingFactor++;
			draw = true;
		}
		else if (reState == RotaryEncoderState::anti_clockwise) {
			coolingFactor--;
			draw = true;
		}
		if (coolingFactor > 20)
			coolingFactor = 20;
		else if (coolingFactor < 0)
			coolingFactor = 0;
	}
	if(reState == RotaryEncoderState::button) {
		draw = true;
		mode++;
		if(mode >= 2)
			mode = 0;
	}
	if(coolingFactor != lastCoolingFactor) {
		draw = true;
		Serial.print("Cooling Factor: ");
		Serial.print(coolingFactor);
		lastCoolingFactor = coolingFactor;

		int servoPos = map(lastCoolingFactor, 0, 10, 0, 10);
		if(coolingFactor == 0)
			servoPos = 0;
		if(coolingFactor >= 10)
			servoPos = 180;
		servoPos = servoPos * 18;
		int fanSpeed = map(lastCoolingFactor, 10, 20, 0, 100);
		if(fanSpeed < 0)
			fanSpeed = 0;
		servo.setPos(180 - servoPos);
		fan.setSpeed(fanSpeed);
	}
	/*
	if(servoPos != lastServoPos) {
		if(servoPos > 180)
			servoPos = 0;
		if(servoPos < 0)
			servoPos = 180;
		lastServoPos = servoPos;
		servo.setPos(servoPos);
	}
	*/
	servo.update();
	updateTemps();
	drawScreen();
}
