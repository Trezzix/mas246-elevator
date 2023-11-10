namespace PIDvars //all float since used for calculation
{
  float error = 0;
  float errorDot = 0;
  float errorInt = 0;
  float errorPrev = 0;

  float kp = 4;
  float ki = 0;
  float kd = 1; //need to fix pid, ki breaks it..

  float u = 0;
}

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

int moveElevator(enumElevatorDir dir,const int floorReq) //tell servo what to do, dir useless? //change floorReq to a pointer so it can be changed during the loop
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
      
    //Serial.println("moving");
    timeAccStart = millis();
    //sensorVars::heightMoved = 0.0; //dosent do anything because servo read gives exact pos after start, not relative
    Serial.println("accelerating");
    while(servoTypeState == servoMoveType::servoAcc)
    {
      //purely used for printing:
      sensorVars::heightMoved =((static_cast<float>( readServoPosition() ) / sensorVars::cps) * meterPerRot); //10 rotations for 5m moved  
      currentHeight = sensorVars::heightMoved + (1.0 * floorDist);//add initial floor, should be a variable
      Serial.print("currentHeight:");
      Serial.print(currentHeight);
      Serial.print(",");
      
      tx = (static_cast<float>(millis() - timeAccStart))/1000.0; //current time minus start of acceleration, convert to s
      dt = (tx - tx_prev);//float because used in calcs
      error = (static_cast<float>(floorReq) * floorDist) - currentHeight; //converted to m 
      Serial.print("error:");
      Serial.print(error);
      Serial.print(",");

      u = ((elevatorAcc * tx ) / elevatorSpeed) * accDir; //function for acceleration then converted to 0-1
         
      //Serial.print(" tx (s): ");
      //Serial.print(tx);
      Serial.print("motor_input:");
      Serial.print(u);
      Serial.print(",");
          
      //analogWrite(enable,motorSpeedPWM);
      output = writeServo(u,output,dt);

      Serial.print("current_speed:");
      Serial.println(output);

      tx_prev = tx;
      //once accelerated to speed, start PID control
      if (abs(u) >= elevatorSpeed)
        {
          Serial.println("starting PID");
          servoTypeState = servoMoveType::servoPID;
        }
      checkButton();
    }
    while(servoTypeState == servoMoveType::servoPID) //control with PID of position to : //switch to switch case instead of 2 very similar while loops
      {
        sensorVars::heightMoved =((static_cast<float>( readServoPosition() ) / sensorVars::cps) * meterPerRot); //10 rotations for 5m moved  
        currentHeight = sensorVars::heightMoved + (1.0 * floorDist);//add initial floor, should be a variable
        Serial.print("currentHeight:");
        Serial.print(currentHeight);
        Serial.print(",");

        tx = (static_cast<float>(millis() - timeAccStart)); //time between start and now
        dt = (tx - tx_prev)/1000.0;//converted to s, float because used in calcs

         //Serial.print("currentHeight:");
        //Serial.print(currentHeight);
        //Serial.print(",");

        //Serial.print("height_moved:");
         //Serial.print(sensorVars::heightMoved);
        //Serial.print(",");
          
        error = (static_cast<float>(floorReq) * floorDist) - currentHeight; //converted to m 
        errorDot = (error - errorPrev)/dt;
        errorInt += (error-errorPrev) * dt;

        Serial.print("error:");
        Serial.print(error);
        Serial.print(",");

        u = (kp * error) + (ki * errorInt) + (kd * errorDot);
          
        Serial.print("motor_input:");
        Serial.print(u);
        Serial.print(",");
          
        output = writeServo(u,output,dt); //input needs to be a derivative of position
          
        Serial.print("current_speed:");
        Serial.println(output);

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
      checkButton();
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
  
  Serial.print("motorSpeedPerc");
  Serial.print(motorSpeedPerc);
  Serial.print(",");

  speedDot = ((((motorSpeedPerc - motorPrev)*static_cast<float>(motorSpeedMax))/(dt)) / 255.0)*maxRPS;
  Serial.print("speedDot:");
  Serial.print(speedDot); //m/s^2
  Serial.print(",");
  
  analogWrite(enable,motorSpeedPWM);
  return motorSpeedPerc;
}
