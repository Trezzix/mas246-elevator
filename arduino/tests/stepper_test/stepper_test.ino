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
  int _A = 69; //enable A
  int _A_phase = 68;
  int _B = 67; //enable B
  int _B_phase = 66;
  int _stepsPerRev = 200;
  float desStepperPos; //unused but should be for sensor
  doorState doorCur;
}

//declarations
void stepperInit();
uint8_t writeStepper(const motorState dir,int stepps,const int curStepp,const int steppDelay_us);

//global vars
int i = 2; //temp

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);
  stepperInit(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (i > 0)
  {
    //close the door, do nothing else
    Serial.print("closing door,");
    Serial.print(" door status: ");
    Serial.println(stepperVars::doorCur);
    moveDoor(up); //run once
    Serial.print("door status: ");
    Serial.println(stepperVars::doorCur);
    delay(2000);
    moveDoor(down);
    Serial.print("door status: ");
    Serial.println(stepperVars::doorCur);
    delay(1000);

    //doorState();
    i--;
    Serial.println(i);
  }
  
}
