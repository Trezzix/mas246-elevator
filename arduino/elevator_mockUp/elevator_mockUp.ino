#include <dac.h>

//define pins

enum enumElevatorDir
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

enum motorState
  {
    up,
    down,
    idleMotor
  };
  
enum elevatorState
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
  float elevatorAcc = 1.2; //m/s^2
  float elevatorSpeed = 1.0; //m/s
  float floorDist = 5.0; //m //why is this in servoVars??????????
  float meterPerRot = 1; //m/rotation
  float maxRPS = (11500.0/131.0)/60.0; //rps of weight at 255 PWM, 
  float speedDot = 0; 
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
    int buttonNumber;
    int buttonPress = 0;
    bool buttonState;
    int ledNumber;
    bool ledState;
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
void checkButton();
void stepperInit();
void servoInit();
void servoEncoderInit();
float writeServo(float motorSpeedPerc, int motorPrev, unsigned long dt);
   
void setup() {
  for(int i = 0; i < 10; i++)
  {
    elevatorRequestsCurrent[i] = -1;
    elevatorRequestsAlt[i] = -1;
  }
  // put your setup code here, to run once:
  Serial.begin(9600);
  stepperInit();  
  servoInit();
  servoEncoderInit();
  delay(2000);
  Serial.println("elevator ready at floor 1");
  
  elevator = first_Floor;
}

void loop() {
  switch (elevator) //states
  {
    case first_Floor:
    {
     currentFloor = 1;
     gotoFloor = currentFloor;
     currentHeight = static_cast<float>(currentFloor) * servoVars::floorDist;
     elevator = idle;
    break;
    }
    case idle:
    {
      //if doors are closed, open them
      if (stepperVars::doorCur == doorClosed)
      {
        delay(2000);
        moveDoor(down); // open doors
        delay(1000);
      }
      else
      {
       if (gotoFloor > currentFloor)
        {
          elevator = prepare_up;
          elevatorMoveDir = elevUp;
        }
        else if (gotoFloor < currentFloor)
        {
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
    case prepare_up: // set a direction as up or down
    {
      Serial.println("moving up");
      if (stepperVars::doorCur == doorClosed)
      {
        elevator = moving_algorithm;
      }
      else
      {
        delay(1000);
        moveDoor(up); // close doors
        delay(2000);
      }
      break;
    }
    case prepare_down:
    {
      Serial.println("moving down");
      if (stepperVars::doorCur == doorClosed)
      {
        elevator = moving_algorithm;
      }
      else
      {
        delay(1000);
        moveDoor(up); // close doors
        delay(2000);
      }
      
      break;
    }
    case moving_algorithm:
    {
      //none right now, just go to floor
      Serial.print("gotoFloor: ");
      Serial.print(gotoFloor);
      int elevatormove123 = moveElevator(elevatorMoveDir,gotoFloor); //what does dir even do here....
      if (elevatormove123 > 0)
      {
        currentFloor = elevatormove123;
        Serial.print("move completed, current floor: ");
        Serial.print(currentFloor);
        Serial.print(" current height: ");
        Serial.println(currentHeight);
      }
      else
      {
        Serial.print("move error");
      }
    
       float temp = static_cast<float>( readServoPosition() ) / sensorVars::cps;
       elevator = idle;
       break;
    }
  }

  clearRequest();
  checkButton();
  if (elevatorRequestsCurrent[0] != -1)
  {
    gotoFloor = elevatorRequestsCurrent[0]; //needs to be a pointer so dynamically changed during moves
  }
  else
  {
    gotoFloor = currentFloor;
  }
}

void posPlot()
{

}