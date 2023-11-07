using namespace servoVars;

void servoInit()
{
  pinMode(enable,OUTPUT);
  pinMode(phase,OUTPUT);
  pinMode(decay,OUTPUT);
  digitalWrite(decay,LOW); // sets to slow in decay mode
}

void moveElevator(motorState dir,int floorReq) //move to servo dingus
{
  //protection so not going up when floor is below or vice-versa? does it need to know the floor?

  if (dir == down)
  {
    //accelerate:
    
    //constant speed

    //decelerate

    //get to correct position with sensor
    
  }
  else
  {
    //same as above, opposite direction
  }
}

void writeServo(motorState dir,int motorSpeed,bool disable)
{
    unsigned long timeAccStart = millis(); //start of acceleration
    digitalWrite(phase, LOW); //why?
    analogWrite(enable,motorSpeed); //enable motor
    delay(2000); //replace with us time or milis

    
    if (disable)
    {
      analogWrite(enable,0); //stop motor
    }
    else
    {
      //do nothing, for smooth acceleration
    }
}
