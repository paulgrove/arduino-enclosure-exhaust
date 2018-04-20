#ifndef rotaryEncoder_h
#define rotaryEncoder_h

enum class RotaryEncoderState {
	none,
	clockwise, anti_clockwise, button
};

class RotaryEncoder {
	public:
		RotaryEncoder(int a, int b, int sw);
		void start();
		RotaryEncoderState read();
	private:
		int _a;
		int _b;
		int _sw;
		int _pinALast;
		RotaryEncoderState _returnLast;
		unsigned long _lastActivation;
};

#endif
