#include <dac.h>

//define pins

enum enumFloors
{
    firstUp,
    secondUp,
    secondDown,
    thirdUp,
    thirdDown,
    fourthDown,
    first,
    second,
    third,
    fourth,
    none
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
    moving_algorithm //?
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
  int motorspeed = 64; //max 255
  int enable = 7;
  int phase = 6;
  int decay = 5;
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
  int currentfloor;
  enumFloors buttonPressed;
  int gotoFloor;//temporary, change for future queue system
  unsigned long timeBegin = millis(); //for time keeping
//buttons

//function prototypes:
enumFloors buttonRead(const int _buttonPress);
void stepperInit();
void servoInit();
   
void setup() {
  // put your setup code here, to run once:
  stepperInit();  
  servoInit();
  
  elevator = first_Floor;
  Serial.begin(9600);
}

void loop() {
  switch (elevator) //states
  {
    case first_Floor:
    {
     currentfloor = 1;
     elevator = idle;
    break;
    }
    case idle:
    {
      if (gotoFloor > currentfloor)
      {
        elevator = prepare_up;
      }
      else if (gotoFloor < currentfloor)
      {
        elevator = prepare_down;
      }
      else
      {
        //do nothing
      }
      break;
    }
    case prepare_up:
    {
      moveDoor(up);
      break;
    }
    case prepare_down:
    {
      moveDoor(down);
      break;
    }
    case moving_algorithm:
    {
      //none right now, just go to floor
      break;
    }
  }
  
  //check buttons: put all in buttonRead func?
  if (Serial.available() > 0 || Serial.read() != 10) //char "10" = enter, can be ignored
    {
      // read the incoming byte:
      HMIvars::buttonPress = Serial.read(); //holy ugly
      buttonPressed = buttonRead(HMIvars::buttonPress);

      if (buttonPressed != none)
      {
          queueSystem(buttonPressed);
      }
      
      //write input:
      Serial.print("Input: ");
	    Serial.println(HMIvars::buttonPress, DEC); 
    }
}

void posPlot()
{

}

void queueSystem(enumFloors desiredFloor)
{
  //temporary, very simple, ignores multiple key presses only looks at newest press and goes to that floor
  switch (desiredFloor)
  {
    case first: case firstUp:
    {
      gotoFloor = 1;
      break;
    }
    case second: case secondUp: case secondDown:
    {
      gotoFloor = 2;
      break;
    }
    case third: case thirdUp: case thirdDown:
    {
      gotoFloor = 3;
      break;
    }
    case fourth: case fourthDown:
    {
      gotoFloor = 4;
      break;
    }
  }
}
