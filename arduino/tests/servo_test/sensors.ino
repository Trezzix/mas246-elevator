using namespace sensorVars;

float servoPos;
float stepperPos;

void servoEncoderInit()
{
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  //attach ISR
  attachInterrupt(digitalPinToInterrupt(encoderPinA),doSignalA,CHANGE);

  //initialize A and B set
  A_set = !digitalRead(encoderPinA);
  B_set = !digitalRead(encoderPinB);
}

long readServoPosition()
{
  noInterrupts();
  long a = encoderPos;
  interrupts();
  return a;
}

void readStepperPos()
{
    //remove from UML also, dosent exist...? only dependant on number of stepps ran
}

void doSignalA()
{
  //read input
  if (digitalRead(encoderPinB) == A_set)
  {
    encoderPos--; //cw, has 131:1 gear ratio also
  }
  else
  {
    encoderPos++; //ccw
  }
  A_set = !A_set;
}

/*
void doSignalB()
{
  //read input
  if (digitalRead(encoderPinA) == B_set)
  {
    encoderPos--;
  }
  else
  {
    encoderPos++;
  }
  B_set = !B_set;
}
*/
