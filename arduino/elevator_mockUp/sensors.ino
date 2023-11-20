using namespace sensorVars;

void servoEncoderInit() //initialize encoder
{
  pinMode(encoderPinA, INPUT_PULLUP); //set as input and enable pullup for sinking
  pinMode(encoderPinB, INPUT_PULLUP);

  //attach ISR
  attachInterrupt(digitalPinToInterrupt(encoderPinA),doSignalA,CHANGE);

  //initialize A and B set
  A_set = !digitalRead(encoderPinA);
  B_set = !digitalRead(encoderPinB);
}

long readServoPosition()
{
  noInterrupts(); //disable interrupts while checking value
  long a = encoderPos;
  interrupts();
  return a;
}

void readStepperPos()
{
    //no sensor for the stepper
}

void doSignalA() //interrupt function
{
  //read input
  if (digitalRead(encoderPinB) == A_set) //if A and B are both on, then it is cw
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
void doSignalB() //untested, not needed
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
