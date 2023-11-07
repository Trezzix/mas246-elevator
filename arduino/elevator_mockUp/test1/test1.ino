#include <dac.h>

bool a = true;

enum doorState
  {
    open,
    closed,
    half
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
  int _A = 69;
  int _A_phase = 68;
  int _B = 67;
  int _B_phase = 66;
  int _stepsPerRev = 200;
  float desStepperPos;
  doorState doorCur;
}

void setup() {
  // put your setup code here, to run once:
  //pinmode stuff for stepper:
  pinMode(stepperVars::_A, OUTPUT);
  pinMode(stepperVars::_B, OUTPUT);
  pinMode(stepperVars::_A_phase, OUTPUT);
  pinMode(stepperVars::_B_phase, OUTPUT);
  dac_init();
  set_dac(4095,4095);//IA per motor phase

  
  Serial.begin(9600);
}

void loop() {
  //close doors:
  
  if (a)
  {
    delay(500);
    Serial.println("test began");
    stepperVars::doorCur = half;
    stepperVars::currentStep = writeStepper(up,20,stepperVars::currentStep,10000);
    stepperVars::doorCur = closed;
    a = false; //run once
    Serial.println(stepperVars::currentStep);

    digitalWrite(stepperVars::_A, LOW); //turn off
    digitalWrite(stepperVars::_A_phase, LOW);
    digitalWrite(stepperVars::_B, LOW);
    digitalWrite(stepperVars::_B_phase, LOW);
  }
}
