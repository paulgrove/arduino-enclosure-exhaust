// include the library code:
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "rotaryEncoder.h"
#include "Fan4Pin.h"
#include "ServoController.h"

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

void setup() {
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

int mode = 0;
int lastMode = -1;
float temp = 0;
float lastTemp = -1;
unsigned long lastSensorPollTime = -1000;
int coolingFactor = 0;
int lastCoolingFactor = -1;
int targetTemp = 35;
int lastTargetTemp = -1;

void drawScreen() {
	if(mode != lastMode) {
		lastMode = mode;
		lcd.clear();
		if(mode == 0) {
			lcd.print("Target Temp");
		}
		else if(mode == 1) {
			lcd.print("Cooling Factor");
		}
	}
	lcd.setCursor(0, 1);
	if(mode == 0) {
		lcd.print(targetTemp);
	}
	else if(mode == 1) {
		lcd.print(coolingFactor * 5);
	}
	lcd.print("	  ");
	if(lastTemp != temp) {
		lastTemp = temp;
		lcd.setCursor(8, 1);
		lcd.print(temp);
	}
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
	RotaryEncoderState reState = rEncoder.read();
	if(mode == 0) {
		if(reState == RotaryEncoderState::clockwise)
			targetTemp++;
		else if (reState == RotaryEncoderState::anti_clockwise)
			targetTemp--;
		if (targetTemp > 60)
			targetTemp = 60;
		else if (targetTemp < 0)
			targetTemp = 0;
	}
	else if (mode == 1) {
		if(reState == RotaryEncoderState::clockwise)
			coolingFactor++;
		else if (reState == RotaryEncoderState::anti_clockwise)
			coolingFactor--;
		if (coolingFactor > 20)
			coolingFactor = 20;
		else if (coolingFactor < 0)
			coolingFactor = 0;
	}
	if(reState == RotaryEncoderState::button) {
		mode++;
		if(mode >= 2)
			mode = 0;
	}
	if(coolingFactor != lastCoolingFactor) {
		lastCoolingFactor = coolingFactor;
		int servoPos = map(lastCoolingFactor, 1, 20, 1, 5);
		if(coolingFactor == 0)
			servoPos = 0;
		servoPos = servoPos * 36;
		int fanSpeed = map(lastCoolingFactor, 0, 20, 0, 100);
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
