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

namespace HMIvars
{
    int buttonNumber;
    int buttonPress = 0;
    bool buttonState;
    int ledNumber;
    bool ledState;
}

//global variables:
  int currentfloor;
  enumFloors buttonPressed;
  int gotoFloor;//temporary, change for future queue system
  unsigned long timeBegin = millis(); //for time keeping
//buttons

enumFloors buttonRead(const int _buttonPress);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:


    //check buttons: put all in buttonRead func?
  if (Serial.available() > 0) //char "10" = enter, can be ignored
    {
      // read the incoming byte:
      HMIvars::buttonPress = Serial.read();
      buttonPressed = buttonRead(HMIvars::buttonPress);

      if (buttonPressed != none)
      {
          queueSystem(buttonPressed);
      }
      
      //write input:
      Serial.print("Input: ");
	    Serial.println(HMIvars::buttonPress, DEC); 
      Serial.println(gotoFloor);
    }
    
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