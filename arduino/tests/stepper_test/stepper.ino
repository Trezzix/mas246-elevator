using namespace stepperVars;

void stepperInit() //does nothing?
{
  pinMode(_A, OUTPUT);
  pinMode(_B, OUTPUT);
  pinMode(_A_phase, OUTPUT);
  pinMode(_B_phase, OUTPUT);
  dac_init();
  set_dac(4095,4095);//IA per motor phase
  currentStep = 0;
}

void moveDoor(motorState dir) //dir: up = open, down = close //should be moved to stepper?
{
  doorCur = doorHalf;
  currentStep = writeStepper(dir,5,currentStep,10000); //speed, acceleration
  if(dir == up)
  {
    doorCur = doorClosed;
  }
  else
  {
    doorCur = doorOpen;
  }
  
}

void closeDoor()
{
  //close doors:
  doorCur = doorHalf;
  currentStep = writeStepper(up,20,currentStep,100000);
  doorCur = doorClosed;
}

void openDoor()
{
  //close doors:
  doorCur = doorHalf;
  currentStep = writeStepper(up,10,stepperVars::currentStep,2500);
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
        Serial.println("ran stepp 1");
        digitalWrite(_A, HIGH); //step 1
        digitalWrite(_A_phase, LOW);
        digitalWrite(_B, HIGH);
        digitalWrite(_B_phase, LOW);
        doorCur = doorOpen;
        break;
      }
      case 1:
      {
        Serial.println("ran stepp 2");
        digitalWrite(_A, HIGH); //step 2
        digitalWrite(_A_phase, LOW);
        digitalWrite(_B, HIGH);
        digitalWrite(_B_phase, HIGH);
        break;
      }
      case 2:
      {
        Serial.println("ran stepp 3");
        digitalWrite(_A, HIGH); //step 3
        digitalWrite(_A_phase, HIGH);
        digitalWrite(_B, HIGH);
        digitalWrite(_B_phase, HIGH);
        break;
      }
      case 3:
      {
        Serial.println("ran stepp 4");
        digitalWrite(_A, HIGH); //step 4
        digitalWrite(_A_phase, HIGH);
        digitalWrite(_B, HIGH);
        digitalWrite(_B_phase, LOW);
        break;
      }
    }

    switch(dir)
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
    Serial.println(stepps);
    stepps--;
    delayMicroseconds(steppDelay_us);
  }
  return steppNum; //steppNum = ny curStepp
}

void tempDoorDisable()
{
  digitalWrite(_A, LOW); //step 4
  digitalWrite(_A_phase, LOW);
  digitalWrite(_B, LOW);
  digitalWrite(_B_phase, LOW);
}
