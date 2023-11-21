enum servoMoveType //type of movement of elevator
{
  servoAcc,
  servoPID,
  moveComplete
};

using namespace PIDvars;
using namespace servoVars;

void servoInit()//initialize servo
{
  pinMode(enable,OUTPUT);
  pinMode(phase,OUTPUT);
  pinMode(decay,OUTPUT);
  digitalWrite(decay,LOW); // sets to slow in decay mode, could be switched to bipolar for simpler PID use
  analogWrite(enable,0);

  motorSpeedMax = static_cast<int>((elevatorSpeed/maxRPS)*255.0); //174
}

int moveElevator(enumElevatorDir dir) //move elevator
{
  bool joltFix = false;//fix for derivative term jump on request change
  float output = 0.0;
  float tx = 0; //time gets converted to float for math
  float tx_prev = 0;
  float dt = 0.0;
  unsigned long timeAccStart;
  servoMoveType servoTypeState = servoAcc; // start movement with acceleration
  Serial.print(" start height: ");//print initial values before move
  Serial.print(currentHeight);
  Serial.print(" height moved: ");
  Serial.println(sensorVars::heightMoved);

  unsigned long finTime;
  bool endingMove = false;
  float accKiDir = 1.0;
  
  if (true)//check if doors are closed with sensor, no sensor on stepper..
  {
    switch (dir)
    {
      case elevUp://change acceleration direction
      {
        accKiDir = 1.0;
        break;
      }
      case elevDown:
      {
        accKiDir = -1.0;
        break;
      }
    }

    timeAccStart = millis(); //set initial time right before move
    Serial.println("Accelerating");
    while(servoTypeState == servoMoveType::servoPID || servoTypeState == servoMoveType::servoAcc)
      {//ends loop if servoTypeState == moveComplete
        floorReq = (elevatorRequestsCurrent[0]);
        sensorVars::heightMoved =((static_cast<float>( readServoPosition() ) / sensorVars::cps) * meterPerRot); //10 rotations for 5m moved  
        currentHeight = sensorVars::heightMoved;//add initial floor, should be a variable
        tx = (static_cast<float>(millis() - timeAccStart))/1000.0; //s
        dt = (tx - tx_prev);//converted to s, float because used in calcs
      
        error = (static_cast<float>(floorReq) * floorDist) - currentHeight - floorDist; //converted to m 
        errorDot = (error - errorPrev)/dt;
        errorInt += (error-errorPrev) * dt;

        switch (servoTypeState)  //movement types:
        {
          case servoAcc:
           {
             u = ((elevatorAcc * tx) / elevatorSpeed) * accKiDir;  //function for acceleration then converted to 0-1 by dividing by max speed
             //Serial.println(u);
             if (abs(u) >= 1.0) {//start pid after accelerated to max speed (100%)
                Serial.println("starting PID");
               servoTypeState = servoMoveType::servoPID;
             }
             break;
           }
         case servoPID:
           {
            if(checkButton()==1 || joltFix)
            {
              Serial.print("Floor request changed, moving to new floor: ");
              Serial.println(floorReq);
              //to prevent jolting on floor change due to derivative, the if statement turns off derivative on the 2nd run which removes the jolt as the error changes abruptly
              if (joltFix == false)
              {
                joltFix = true;
              }
              else
              {
                joltFix = false;
                errorDot = 0;
              }
            }
            u = (kp * error) + (ki * errorInt) + (kd * errorDot) + (deadzone * accKiDir);//PID calculation with deadzone
            break;
           }
        }

        posPlot(currentHeight,error,u); //print plot
          
        output = writeServo(u,output,dt); //drive servo, output and dt are used for acceleration printing
        posPlot(output);//print plot

        errorPrev = error;
        tx_prev = tx;

        //check if on floor for 2 seconds
        if ((error <= 0.025) && (error >= -0.025) && (endingMove == false)) //if the elevator is within 0.025m of the floor
        {
          finTime = millis();
          endingMove = true;
        }
        if(((millis() - finTime) >= 2000) && (endingMove))
        {
          if ((error <= 0.015) && (error >= -0.015))
          {
            servoTypeState = moveComplete; //exit loop when its been within 0.025m of the floor for 2 seconds
          }
          else
          {
            endingMove = false;
          }
        }
      
    }
    analogWrite(enable,0); //turn of motor if not already off
    return floorReq; //return what floor it just moved to
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
  
  motorSpeedPWM = static_cast<int>(abs(motorSpeedPerc) * motorSpeedMax);//convert 0-1 to PWM based on calculated max pwm
  
  //if (speedDot >= )

  speedDot = ((((motorSpeedPerc - motorPrev)*static_cast<float>(motorSpeedMax))/(dt)) / 255.0)*maxRPS; //calculate acceleration

  posPlot(motorSpeedPerc,speedDot);//print acceleration and speed to graph
  
  analogWrite(enable,motorSpeedPWM);//move DC motor
  return motorSpeedPerc;//return current speed for next loop to calculate acceleration
}
