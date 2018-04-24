#include "ServoController.h"
#include "Arduino.h"

ServoController::ServoController(int pin, int startF, int endF) {
	_pin = pin;
	_startF = startF;
	_endF = endF;
	this->setPos(0);
};

void ServoController::start() {
	_servo.attach(_pin, _startF, _endF);
}

void ServoController::setPos(int pos) {
	if(!_activated) {
		_activated = true;
		_servo.attach(_pin, _startF, _endF);
	}
	_updatedTime = millis();
	_pos = pos;
	_servo.write(_pos);
}

void ServoController::update() {
	if(_activated) {
		unsigned long now = millis();
		if (millis() - _updatedTime > 1000) {
			_servo.detach();
			_activated = false;
		}
	}
}
