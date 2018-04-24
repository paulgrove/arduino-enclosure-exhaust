#ifndef ServoController_h
#define ServoController_h

#include "Servo.h"

class ServoController {
	public:
		ServoController(int pin, int startF, int endF);
		void start();
		void setPos(int pos);
		void update();
	private:
		Servo _servo;
		bool _activated;
		int _pin;
		int _startF;
		int _endF;
		int _pos;
		unsigned long _updatedTime;
};

#endif
