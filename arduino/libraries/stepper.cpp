#include <> //matte
#include "Arduino.h"

int stepper([enum] dir,int stepps,int curStepp,int steppDelay)
{
	if (stepps > 0x1000 | curStepp > 4)
	{
		stepps = 0; //error, ends func
		return curStepp;
	}
	uint32_t a = 0x1000 + curStepp;
	//uint32_t compare = 0x4; //istedefor senere?
	while(stepps > 0)
	{
		b = a & 0x4;
		case b
		switch 1:
		{
			//stepper kode her
			break;
		}
		switch 2:
		{
			
			break;
		}
		switch 3:
		{
			
			break;
		}
		switch 4:
		{
			
			break;
		}
		a++; //or -- depending on dir, use bool multiplication or int thats 1 or -1
		stepps--;
		delay(steppDelay);
	}
	return b; //b = curStepp her
}