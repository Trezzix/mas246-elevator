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
  int motorSpeedMax = 0; //max 255
  const int enable = 7;
  const int phase = 6;
  const int decay = 5; // should be defined?
  float elevatorAcc = 1.2; //m/s^2
  float elevatorSpeed = 1.0; //m/s
  float floorDist = 5.0; //m
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
  int currentFloor;
  motorState elevatorDir; //move to servoVars namespace
  float currentHeight; //move to servoVars namespace
  enumFloors buttonPressed; //move to HMI namespace
  int gotoFloor;//temporary, change for future queue system
  unsigned long timeBegin = millis(); //for time keeping
//buttons

//function prototypes:
enumFloors buttonRead(const int _buttonPress);
void stepperInit();
void servoInit();
void servoEncoderInit();
float writeServo(float motorSpeedPerc, int motorPrev, unsigned long dt);
   
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  stepperInit();  
  servoInit();
  servoEncoderInit();
  
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
      if (gotoFloor > currentFloor)
      {
        elevator = prepare_up;
        elevatorDir = up;
      }
      else if (gotoFloor < currentFloor)
      {
        elevator = prepare_down;
        elevatorDir = down;
      }
      else
      {
        //do nothing
      }
      break;
    }
    case prepare_up: // set a direction as up or down
    {
      Serial.println("moving up");
      //moveDoor(up); // close doors
      elevator = moving_algorithm;
      break;
    }
    case prepare_down:
    {
      Serial.println("moving down");
      //moveDoor(up); //close doors
      elevator = moving_algorithm;
      break;
    }
    case moving_algorithm:
    {
      //none right now, just go to floor
      Serial.print("gotoFloor: ");
      Serial.print(gotoFloor);
      int elevatormove123 = moveElevator(elevatorDir,gotoFloor); //what does dir even do here....
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
  
  //check buttons: put all in buttonRead func?
  if (Serial.available() > 0) //char "10" = enter, can be ignored
    {
      // read the incoming byte:
      HMIvars::buttonPress = Serial.read();
      buttonPressed = buttonRead(HMIvars::buttonPress);

      if (buttonPressed != none)
      {
          queueSystem(buttonPressed);
          Serial.print("button: moving to floor: ");
          Serial.println(gotoFloor);
          //write input:
      }
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
