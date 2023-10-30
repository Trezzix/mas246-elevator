//needs protections?
#include <Arduino.h>

namespace motorEnums
{
	enum motorState{winding,unwinding,stopped};
}

class MotorStepper
{
public:
	uint8_t writeStepper(motorEnums::motorState dir,int stepps,int curStepp,int steppDelay_us);
	MotorStepper(int Apin,int APhasePin, int Bpin, int BPhasePin);
private:
	//servo pins
	const int _A; //set in constructor, put _ after variables not before...
	const int _A_phase;
	const int _B;
	const int _B_phase;
	const int _stepsPerRev = 200; //unused for now
};

class MotorDC
{
public:
	void writeDC(motorEnums::motorState dir, int driveSpeed,int driveTime_ms);
	MotorDC(int pinEnable, int pinPhase, int pinDecay);
private:
	//dc pins
	const int enable_;
	const int phase_;
	const int decay_;
	//int motorspeed = 255;// why exist
};