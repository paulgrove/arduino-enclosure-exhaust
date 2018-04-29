#include "rotaryEncoder.h";
#include "Arduino.h"

RotaryEncoder::RotaryEncoder(int a, int b, int sw) {
	_a = a;
	_b = b;
	_sw = sw;
	_pinALast = 0;
	_returnLast = RotaryEncoderState::none;
	unsigned long _lastActivation = millis();
}

void RotaryEncoder::start() {
	pinMode(_a, INPUT);
	pinMode(_b, INPUT);
	pinMode(_sw, INPUT);
	_pinALast = digitalRead(_a);
}

RotaryEncoderState RotaryEncoder::read() {
	boolean bCW;
	RotaryEncoderState returnVal = RotaryEncoderState::none;
	int swVal = digitalRead(_sw);
	if (swVal == LOW) {
		unsigned long thisActivation = millis();
		if(thisActivation - _lastActivation > 250) {
			_lastActivation = thisActivation;
			return RotaryEncoderState::button;
		}
	}
	int aVal = digitalRead(_a);
	if (aVal != _pinALast) {
		unsigned long thisActivation = millis();
		if(thisActivation - _lastActivation > 40) {
			_lastActivation = thisActivation;
			// knob is rotating - get direction
			if (digitalRead(_b) != aVal) {
				bCW = true;
				returnVal = RotaryEncoderState::clockwise;
			} else {
				bCW = false;
				_lastActivation = millis();
				returnVal = RotaryEncoderState::anti_clockwise;
			}
		}
	}
	_pinALast = aVal;
	if(_returnLast == returnVal) {
		return RotaryEncoderState::none;
	}
	_returnLast = returnVal;
	return returnVal;
}
