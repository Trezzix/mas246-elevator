namespace PIDvars //all float since used for calculation
{
  float error = 0;
  float errorDot = 0;
  float errorInt = 0;
  float errorPrev = 0;

  float kp = 1.5;
  float ki = 0;
  float kd = 1;

  float u = 0;
  float uDot = 0;
}

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

void moveElevator(motorState dir,int floorReq) //tell servo what to do, dir useless?
{
  //protection so not going up when floor is below or vice-versa? does it need to know the floor?
  float output = 0.0;
  unsigned long tx = 0;
  unsigned long tx_prev = 0;
  float dt = 0.0;
  unsigned long timeAccStart;
  float startHeight = currentHeight;
  
  if (true)//check if doors are closed with sensor
  {
    switch (dir)
    {
      case up:
      {
        //Serial.println("moving");
        timeAccStart = millis();
        floorReq = floorReq * floorDist;

        while(true) //control with PID of position to :
        {
          sensorVars::heightMoved = ((static_cast<float>( readServoPosition() ) / sensorVars::cps) * meterPerRot); //10 rotations for 5m moved  
          currentHeight = sensorVars::heightMoved + startHeight;         
          tx = (static_cast<float>(millis() - timeAccStart)); //time between start and now
          dt = static_cast<float>(tx - tx_prev)/1000.0;//converted to s, float because used in calcs

          //Serial.print("currentHeight:");
          //Serial.print(currentHeight);
          //Serial.print(",");

          //Serial.print("height_moved:");
          //Serial.print(sensorVars::heightMoved);
          //Serial.print(",");
          
          error = (static_cast<float>(floorReq)) - currentHeight; //converted to m 
          errorDot = (error - errorPrev)/dt;
          errorInt += (error-errorPrev) * dt;

          Serial.print("error:");
          Serial.print(error);
          Serial.print(",");

          u = (kp * error) + (ki * errorInt) + (kd * errorDot);
          
          Serial.print("PID_output:");
          Serial.print(u);
          Serial.print(",");
          
          output = writeServo(u,output,dt); //input needs to be a derivative of position
          
          Serial.print("current_speed:");
          Serial.println(output);
          errorPrev = error;
          tx_prev = tx;
          //check if new floorReq?
        }
        //Serial.println("move completed");
      }
      case down:
      {
        
      }
    }
  }
  else
  {
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
  
  Serial.print("motorSpeedPerc");
  Serial.print(motorSpeedPerc);
  Serial.print(",");

  speedDot = ((((motorSpeedPerc - motorPrev)*static_cast<float>(motorSpeedMax))/(dt)) / 255.0)*maxRPS;
  Serial.print("speedDot:");
  Serial.print(speedDot); //unit?
  Serial.print(",");
  
  analogWrite(enable,motorSpeedPWM);
  return motorSpeedPerc;
}
