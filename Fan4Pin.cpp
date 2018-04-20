#include "Fan4Pin.h";
#include "Arduino.h"

void Fan4Pin::start() {
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
}

void Fan4Pin::setSpeed(int speed) {
	if(_speed != speed) {
		_speed = speed;
		if(_speed < 0)
			_speed = 100;
		if(_speed > 100)
			_speed = 0;
    	OCR2B = map(_speed, 0, 100, 0, 79);
	}
}

int Fan4Pin::getSpeed() {
	return _speed;
}
