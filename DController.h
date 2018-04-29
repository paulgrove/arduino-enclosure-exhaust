#ifndef DController_h
#define DController_h

class DController {
	public:
		DController(unsigned long sampleRate, unsigned long tRangeLow, unsigned long tRangeHigh);
		int update(float temp, float target);
	private:
		unsigned long _lastTime;
		unsigned long _sampleRate;
		float _lastTemp;
		unsigned long _tRangeLow;
		unsigned long _tRangeHigh;
};

#endif
