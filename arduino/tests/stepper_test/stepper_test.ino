#include <dac.h>

enum doorState
  {
    doorOpen,
    doorClosed,
    doorHalf
  };

enum motorState
  {
    up,
    down,
    idleMotor
  };

namespace stepperVars
{  
  int currentStep = 0;
  int _A = 69; //should be replaced wth #define?
  int _A_phase = 68;
  int _B = 67;
  int _B_phase = 66;
  int _stepsPerRev = 200;
  float desStepperPos; //unused but should be for sensor
  doorState doorCur;
}

//declarations
void stepperInit();
uint8_t writeStepper(const motorState dir,int stepps,const int curStepp,const int steppDelay_us);

//global vars
bool a = true; //temp

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  stepperInit();  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (a)
  {
    //close the door, do nothing else
    Serial.print("closing door,");
    Serial.print(" door status: ");
    Serial.println(stepperVars::doorCur);
    closeDoor(); //run once
    Serial.print("door status: ");
    Serial.println(stepperVars::doorCur);

    tempDoorDisable();

    //doorState();
    a = false;
  }
  
}
