enum servoMoveType
{
  servoAcc,
  servoPID,
  moveComplete
};

using namespace PIDvars;
using namespace servoVars;

void servoInit()
{
  pinMode(enable,OUTPUT);
  pinMode(phase,OUTPUT);
  pinMode(decay,OUTPUT);
  digitalWrite(decay,LOW); // sets to slow in decay mode, maybe switch to bipolar for simpler PID use?
  analogWrite(enable,0);

  motorSpeedMax = static_cast<int>((elevatorSpeed/maxRPS)*255.0); //174
}

int moveElevator(enumElevatorDir dir) //tell servo what to do, dir useless? //change floorReq to a pointer so it can be changed during the loop
{
  //protection so not going up when floor is below or vice-versa? does it need to know the floor?
  float output = 0.0;
  float tx = 0;
  float tx_prev = 0;
  float dt = 0.0;
  unsigned long timeAccStart;
  servoMoveType servoTypeState = servoAcc; // start movement with acceleration
  Serial.print(" start height: ");
  Serial.print(currentHeight);
  Serial.print(" height moved: ");
  Serial.println(sensorVars::heightMoved);

  unsigned long finTime;
  bool endingMove = false;
  float accDir = 1;
  
  if (true)//check if doors are closed with sensor
  {
    switch (dir)
    {
      //do stuffs depending on direction
      case elevUp:
      {
        accDir = 1;
        break;
      }
      case elevDown:
      {
        accDir = -1;
        break;
      }
    }

    timeAccStart = millis();
    Serial.println("Accelerating");
    while(servoTypeState == servoMoveType::servoPID || servoTypeState == servoMoveType::servoAcc) //control with PID of position to : //switch to switch case instead of 2 very similar while loops
      {
        floorReq = (elevatorRequestsCurrent[0]);
        sensorVars::heightMoved =((static_cast<float>( readServoPosition() ) / sensorVars::cps) * meterPerRot); //10 rotations for 5m moved  
        currentHeight = sensorVars::heightMoved;//add initial floor, should be a variable
        tx = (static_cast<float>(millis() - timeAccStart))/1000.0;
        dt = (tx - tx_prev);//converted to s, float because used in calcs
      
        error = (static_cast<float>(floorReq) * floorDist) - currentHeight - floorDist; //converted to m 
        errorDot = (error - errorPrev)/dt;
        errorInt += (error-errorPrev) * dt;

        switch (servoTypeState)  //movement types:
        {
          case servoAcc:
           {
             u = ((elevatorAcc * tx) / elevatorSpeed) * accDir;  //function for acceleration then converted to 0-1
             //Serial.println(u);
             if (abs(u) >= elevatorSpeed) {
                Serial.println("starting PID");
               servoTypeState = servoMoveType::servoPID;
             }
           }
         case servoPID:
           {
            if(checkButton()==1)
            {
              Serial.print("Floor request changed, moving to floor: ");
              Serial.println(floorReq);
              errorDot = 0; //to prevent jolting on error change
            }
             u = (kp * error) + (ki * errorInt) + (kd * errorDot);
           }
        }

        posPlot(currentHeight,error,u);
          
        output = writeServo(u,output,dt); //drive servo
        posPlot(output);

        errorPrev = error;
        tx_prev = tx;
        //check if new floorReq?

        //check if on floor for 2 seconds (there has to be a better way to do this.......)
        if ((error <= 0.025) && (error >= -0.025) && (endingMove == false)) //replace with short if?
        {
          finTime = millis();
          endingMove = true;
        }
        if(((millis() - finTime) >= 2000) && (endingMove))
        {
          if ((error <= 0.015) && (error >= -0.015))
          {
            servoTypeState = moveComplete; //exit loop
          }
          else
          {
            endingMove = false;
          }
        }
      
    }
    analogWrite(enable,0); //servo elevator
    return floorReq;
  }
  else
  {
    return 0;
    //error: doors not closed
  }
}

float writeServo(float motorSpeedPerc, float motorPrev, float dt) //input: -1 to 1,, should include a stop function?
{  
  bool servoDir = false;
  int motorSpeedPWM =0;
  //float maxAccPWM = (elevatorAcc/meterPerRot));
  if (motorSpeedPerc >= 1) // capping to max speed
  {
    motorSpeedPerc = 1.0;
  }
  else if (motorSpeedPerc <= -1)
  {
    motorSpeedPerc = -1.0;
  }

  servoDir = (motorSpeedPerc >= 0); //true if positive, false if negative
  digitalWrite(phase,!servoDir);
  
  motorSpeedPWM = static_cast<int>(abs(motorSpeedPerc) * motorSpeedMax);
  
  //if (speedDot >= )

  speedDot = ((((motorSpeedPerc - motorPrev)*static_cast<float>(motorSpeedMax))/(dt)) / 255.0)*maxRPS;

  posPlot(motorSpeedPerc,speedDot);
  
  analogWrite(enable,motorSpeedPWM);
  return motorSpeedPerc;
}
