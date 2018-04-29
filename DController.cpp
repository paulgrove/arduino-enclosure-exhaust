#include "DController.h"
#include "Arduino.h"

DController::DController(unsigned long sampleRate, unsigned long tRangeLow, unsigned long tRangeHigh) {
	_sampleRate = sampleRate;
	_lastTime = 0;
	_tRangeLow = tRangeLow;
	_tRangeHigh = tRangeHigh;
	_lastTemp = 0;
}

int DController::update(float temp, float target) {
	unsigned long now = millis();

	if(now - _lastTime < _sampleRate)
		return 0;

	if(_lastTime == 0) {
		_lastTemp = temp;
		_lastTime = now;
		return 0;
	}

	float slope = (_lastTemp - temp) / ((_lastTime / 1000) - (now / 1000));
	float tempDiff = target - temp;
	float timeToTargetSec = tempDiff / slope;

	Serial.println("Temp CTRL");
	Serial.print("slope ");
	Serial.print(slope);
	Serial.print(" = (");
	Serial.print(_lastTemp);
	Serial.print(" - ");
	Serial.print(temp);
	Serial.print(") / (");
	Serial.print(_lastTime / 1000);
	Serial.print(" - ");
	Serial.print(now / 1000);
	Serial.println(")");
	Serial.print("timeToTarget ");
	Serial.println(timeToTargetSec);
	Serial.print("tempDiff ");
	Serial.println(tempDiff);
	
	_lastTemp = temp;
	_lastTime = now;

	int returnv = 0;

	if(slope < 0.001 && slope > -0.001) {
		if(tempDiff > 0)
			returnv = -1; // decrease coolingFactor
		else if(tempDiff < 0)
			returnv = 1; //increase coolingFactor
	}
	else if(timeToTargetSec < _tRangeLow / 1000) {
		if(tempDiff > 0)
			returnv = -1; // decrease coolingFactor
		else if(tempDiff < 0)
			returnv = 1; //increase coolingFactor
	}
	else if(timeToTargetSec > _tRangeHigh / 1000) {
		if(tempDiff > 0)
			returnv = 1; // increase coolingFactor
		else if(tempDiff < 0)
			returnv = -1; // decrease coolingFactor
	}
	Serial.print("return: ");
	Serial.println(returnv);
	return returnv;
}
