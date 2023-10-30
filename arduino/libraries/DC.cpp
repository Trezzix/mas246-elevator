#include "elevator_func.h"
#include <Arduino.h>

MotorDC::MotorDC(int pinEnable, int pinPhase, int pinDecay)//constructor
:enable_(pinEnable)
,phase_(pinPhase)
,decay_(pinDecay)
{
    //arduino initialize
    pinMode(enable_, OUTPUT);
	pinMode(phase_, OUTPUT);
	pinMode(decay_, OUTPUT);
    digitalWrite(decay_,LOW);
}

void MotorDC::writeDC(motorEnums::motorState dir, int driveSpeed,int driveTime_ms)
{
    if (driveSpeed > 255 | driveSpeed < 0)
    {
        //do nothing, bad values
    }
    else
    {
        switch (dir)
        {
        case motorEnums::winding:
        {
            digitalWrite(phase_,LOW); //clockwise
            break;
        }
        case motorEnums::unwinding:
        {
            digitalWrite(phase_,HIGH); //counter-clockwise
            break;
        }
            
        }
        analogWrite(enable_, driveSpeed);
        delay(driveTime_ms); //replace with millis?
        analogWrite(enable_,0); //turn off motor
    }
}