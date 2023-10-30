//#include <> //matte
#include "elevator_func.h"
#include <Arduino.h>
#include "dac.h"

MotorStepper::MotorStepper(int Apin,int APhasePin, int Bpin, int BPhasePin) //constructor
:_A(Apin)
,_A_phase(Apin)
,_B(Apin)
,_B_phase(Apin)
{
	//arduino set pins as outputs and initialize dac
	pinMode(_A, OUTPUT);
	pinMode(_B, OUTPUT);
	pinMode(_A_phase, OUTPUT);
	pinMode(_B_phase, OUTPUT);
	dac_init();
	set_dac(4095,4095);//IA per motor phase
}

uint8_t MotorStepper::writeStepper(const motorEnums::motorState dir,int stepps,const int curStepp,const int steppDelay_us)
{
	//initial:
	//steppDelayus should be minimum(?) 2500
	//uint8_t compare = 0x3;
	uint8_t base = 0x10; // less overflowing, defined if it does though
	uint8_t steppNum = curStepp;

	if (curStepp > 3)
	{
		stepps = 0; //error, ends func
		return curStepp;
	}

	while(stepps > 0)
	{
		steppNum = base & 0b00000011;

		switch (steppNum)
		{
			case 0:
			{
				digitalWrite(_A, HIGH); //step 1
				digitalWrite(_A_phase, LOW);
				digitalWrite(_B, HIGH);
				digitalWrite(_B_phase, LOW);
				//delay(steppDelay); //replace with millis?
				break;
			}
			case 1:
			{
				digitalWrite(_A, HIGH); //step 2
				digitalWrite(_A_phase, LOW);
				digitalWrite(_B, HIGH);
				digitalWrite(_B_phase, HIGH);
				break;
			}
			case 2:
			{
				digitalWrite(_A, HIGH); //step 3
				digitalWrite(_A_phase, HIGH);
				digitalWrite(_B, HIGH);
				digitalWrite(_B_phase, HIGH);
				break;
			}
			case 3:
			{
				digitalWrite(_A, HIGH); //step 4
				digitalWrite(_A_phase, HIGH);
				digitalWrite(_B, HIGH);
				digitalWrite(_B_phase, LOW);
				break;
			}
		}

		switch(dir)
		{
			case motorEnums::winding:
			{
				base++;
				break;
			}
			case motorEnums::unwinding:
			{
				base--;
				break;
			}
		}
		stepps--;
		delayMicroseconds(steppDelay_us);
	}
	return steppNum; //steppNum = ny curStepp
}