using namespace stepperVars;

void stepperInit() //initialize stepper
{
  pinMode(_A, OUTPUT);
  pinMode(_B, OUTPUT);
  pinMode(_A_phase, OUTPUT);
  pinMode(_B_phase, OUTPUT);
  dac_init(); //enable dac for stepper motor
  set_dac(4095,4095);//IA per motor phase
}

void moveDoor(motorState dir) //dir: up = open, down = close, winding/unwinding would make more sense here
{
  doorCur = doorHalf; //set as half open/closed while moving
  switch (dir)//open/close door depending on input
  {
    case up:
    {
      lcdDisplay(1,"Closing door");
      currentStep = writeStepper(dir,200,currentStep,5000);
      doorCur = doorClosed; //should be a check and not just set after the function is ran
      lcdDisplay(1,"Door closed");
      break;
    }
    case down:
    {
      lcdDisplay(1,"Opening door");
      currentStep = writeStepper(dir,200,currentStep,5000);
      lcdDisplay(1,"Door open");
      doorCur = doorOpen;
      break;
    }
  }  
}

uint8_t writeStepper(const motorState dir,int stepps,const int curStepp,const int steppDelay_us)
{
  //initial:
  //steppDelayus should be minimum(?) 2500
  //uint8_t compare = 0x3;
  uint8_t base = 0x10; // less overflowing, defined if it does though
  uint8_t steppNum = curStepp;

  if (curStepp > 3 || steppDelay_us < 2500)
  {
    stepps = 0; //error, ends function, should return error as door will not be closed
    return curStepp;
  }

  while(stepps > 0)
  {
    steppNum = base & 0b00000011; //makes steppNum loop from 0-3

    switch (steppNum) //run induvidual stepp:
    {
      case 0:
      {
        digitalWrite(_A, HIGH); //step 1
        digitalWrite(_A_phase, LOW);
        digitalWrite(_B, HIGH);
        digitalWrite(_B_phase, LOW);
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

    switch(dir) //increase or decrease base depending on direction, could be optimized
    {
      case up:
      {
        base++;
        break;
      }
      case down:
      {
        base--;
        break;
      }
    }
    stepps--; //count number of stepps ran
    delayMicroseconds(steppDelay_us); //run delay between each stepp
  }
  return steppNum; //steppNum = ny curStepp, return so stepper knows current position for next movement
}
