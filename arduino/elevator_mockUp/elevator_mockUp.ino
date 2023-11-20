#include <LiquidCrystal.h>
#include <dac.h>


//enumerators

enum enumElevatorDir //direction for the buttons to compare to the elevator direction
{
  elevUp,
  elevDown,
  elevNeither,
  reqInternal //internal request is independent of direction
};

enum doorState
  {
    doorOpen,
    doorClosed,
    doorHalf
  };

enum motorState //winding,unwinding,idle
  {
    up,
    down,
    idleMotor
  };
  
enum elevatorState //states for the elevator
  {
    first_Floor,
    idle,
    prepare_up,
    prepare_down,
    moving_algorithm,
    moving_up, //used for other variable
    moving_down
   };

struct buttonPressType
{
  enumElevatorDir bRequestDir;
  int8_t floorNum;
};

//namespaces, supposed to be in induvidual .ino files but it wouldnt compile that way

namespace stepperVars
{  
  int currentStep = 0;
  int _A = 69; //should be replaced wth #define?
  int _A_phase = 68;
  int _B = 67;
  int _B_phase = 66;
  int _stepsPerRev = 200;
  float desStepperPos; //unused but should be for sensor
  doorState doorCur;
}

namespace servoVars
{
  int motorSpeedMax = 0; //max 255
  const int enable = 7;
  const int phase = 6;
  const int decay = 5; // should be defined?
  const float elevatorAcc = 1.2; //m/s^2
  const float elevatorSpeed = 1.0; //m/s
  const float floorDist = 5.0; //m //why is this in servoVars??????????
  const float meterPerRot = 1; //m/rotation
  const float maxRPS = (11500.0/131.0)/60.0; //rps of weight at 255 PWM, 
  float speedDot = 0;
  int floorReq;
}

namespace sensorVars
{
  const byte encoderPinA = 20;
  const byte encoderPinB = 21;
  const float cps(32.0 * 131.0); //float since only used for maths, cps * gear ratio of 131:1
  volatile long encoderPos = 0;
  volatile boolean A_set = true;
  volatile boolean B_set = true;
  float heightMoved = 0.0;
}

namespace HMIvars
{
    int buttonPress = 0;
    int led0 = 49;
    int led1 = 48;
    int led2 = 47;
    int led3 = 46;
    //int ledNumber;
    //bool ledState;
    int LCD_Backlight = 4;
    LiquidCrystal lcd(41,40,37,36,35,34);
}

namespace PIDvars //all float since used for calculation
{
  float error = 0;
  float errorDot = 0;
  float errorInt = 0;
  float errorPrev = 0;

  float kp = 3;
  float ki = -0.00;
  float kd = 0.25; //need to fix pid, ki breaks it..

  float u = 0;
}
   
//global variables:
  elevatorState elevator;
  enumElevatorDir elevatorMoveDir;
  int currentFloor;
  float currentHeight; //move to servoVars namespace
  int gotoFloor;//temporary, change for future queue system
  unsigned long timeBegin = millis(); //for time keeping
  int8_t elevatorRequestsCurrent[10];
  int8_t elevatorRequestsAlt[10];
//buttons

//function prototypes:
buttonPressType buttonRead(const int _buttonPress);
int checkButton();
void stepperInit();
void servoInit();
void servoEncoderInit();
float writeServo(float motorSpeedPerc, int motorPrev, unsigned long dt);
   
void setup() {
  for(int i = 0; i < 10; i++) //set all requests to -1 (-1 = no request)
  {
    elevatorRequestsCurrent[i] = -1;
    elevatorRequestsAlt[i] = -1;
  }
  //run all init functions:
  Serial.begin(19200);
  hmiInit();
  stepperInit();  
  servoInit();
  servoEncoderInit();
  delay(2000);
  Serial.println("elevator ready at floor 1");
  
  elevator = first_Floor;
}

void loop() {
  switch (elevator) //elevator states
  {
    case first_Floor: //initialize elevator
    {
     currentFloor = 1;
     gotoFloor = currentFloor;
     currentHeight = (static_cast<float>(currentFloor) * servoVars::floorDist) - servoVars::floorDist;
     lcdDisplay(0,"Floor: ",currentFloor);
     lcdDisplay(1,"Door open");
     elevator = idle;
    break;
    }
    case idle: //elevator functions begin:
    {
      if (currentFloor == 1) //set dir on lowest floor
      {
        elevatorMoveDir = elevUp;
      }
      else if ((currentFloor == 4)) //set dir on highest floor (4)
      {
        elevatorMoveDir = elevDown;
      }

      if (stepperVars::doorCur == doorClosed) //if doors are closed, open them
      {
        delay(2000);
        moveDoor(down); //open doors
        ledWrite();
        delay(1000);
      }
      else
      {
       if (gotoFloor > currentFloor) //move up
        {
          lcdDisplay(0,"Moving up to: ",gotoFloor);
          elevator = prepare_up;
          elevatorMoveDir = elevUp;
        }
        else if (gotoFloor < currentFloor) //move down
        {
          lcdDisplay(0,"Moving dwn to: ",gotoFloor);
          elevator = prepare_down;
          elevatorMoveDir = elevDown;
        }
        else
        {
          //do nothing
        }
      }
      break;
    }
    case prepare_up: //begin moving up
    {
      Serial.println("moving up");
      if (stepperVars::doorCur == doorClosed) //only move if doors are closed
      {
        elevator = moving_algorithm;
      }
      else
      {
        delay(1000);
        moveDoor(up); //close doors
        delay(2000);
      }
      break;
    }
    case prepare_down:
    {
      Serial.println("moving down");
      if (stepperVars::doorCur == doorClosed) //only move if doors are closed
      {
        elevator = moving_algorithm;
      }
      else
      {
        delay(1000);
        moveDoor(up); //close doors
        delay(2000);
      }
      break;
    }
    case moving_algorithm: //moving algorithm goes up and down 
    {
      Serial.print("gotoFloor: ");
      Serial.print(gotoFloor);
      int elevatormove123 = moveElevator(elevatorMoveDir); //move the elevator, returns 1 if success
      if (elevatormove123 > 0)
      {
        currentFloor = elevatormove123;
        lcdDisplay(0,"Floor: ",currentFloor); //print info
        Serial.print("move completed, current floor: ");
        Serial.print(currentFloor);
        Serial.print(" current height: ");
        Serial.println(currentHeight);
      }
      else //rarely used
      {
        lcdDisplay(1,"Move error");
        Serial.print("move error");
      }
    
       float temp = static_cast<float>( readServoPosition() ) / sensorVars::cps;//not used
       elevator = idle; //open doors and wait for new floor requests
       break;
    }
  }

  clearRequest(); //check if current floor = current request
  checkButton(); //check button each loop
  if (elevatorRequestsCurrent[0] != -1)
  {
    gotoFloor = elevatorRequestsCurrent[0]; //set gotofloor if there is a request
  }
  else
  {
    gotoFloor = currentFloor; //else dont move
  }
}

//graphing for plotting:
void posPlot(float currentHeight,float error,float u)
{
  Serial.print(millis());
  Serial.print(",");

 // Serial.print("currentHeight:");
  Serial.print(currentHeight);
  Serial.print(",");

  //Serial.print("height_moved:");
  //Serial.print(sensorVars::heightMoved);
  //Serial.print(",");

  //Serial.print("error:");
  Serial.print(error);
  Serial.print(",");

  //Serial.print("motor_input:");
  Serial.print(u);
  Serial.print(",");
}
void posPlot(float motorSpeedPerc, float speedDot)
{
 // Serial.print("motorSpeedPerc:");
  Serial.print(motorSpeedPerc);
  Serial.print(",");
  //Serial.print("speedDot:");
  Serial.print(speedDot); //m/s^2
  Serial.print(",");
}
void posPlot(float output)
{
  //Serial.print("current_speed:");
  Serial.println(output);
}
