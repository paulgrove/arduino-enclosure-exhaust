#ifndef Fan4Pin_h
#define Fan4Pin_h

class Fan4Pin {
	public:
		void start();
		void setSpeed(int speed);
		int getSpeed();
	private:
		int _speed;
};

#endif
