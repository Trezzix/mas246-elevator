  //elevator mockup, skeleton for project, simulation for 4 floors

 //enumerators
  enum servoState
  {
    up,
    down,
    idle
  };

  enum stepperState
  {
    open,
    closed,
    half
  };
  
  enum elevatorState
  {
    first_Floor,
    idle,
    prepare_up,
    prepare_down,
    moving_algorithm //?
   };

 //global variables:
  int currentfloor;
  elevatorState elevator;
  volatile floors buttonPressed;
//buttons


void setup() {
  // put your setup code here, to run once:
  elevator = first_Floor;
  serial.begin();
}

void loop() {

  switch (elevator)
  {
    case first_Floor:
    {
     currentfloor = 1;
     elevator = idle;
    break;
    }
    case idle:
    {
      if (1)
      {
        queueSystem();
      }
      else if (0)
      {

      }
      else
      {
        //do nothing
      }
      break;
    }
    case prepare_up:
    {
      break;
    }
    case prepare_down:
    {
      break;
    }
    case moving_algorithm:
    {
      break;
    }
    
    
  }
  //check buttons:
  buttonPressed = buttonRead();
  if (buttonPressed != none)
  {
    queueSystem(buttonPressed);
  }
}

void moveElevator()
{

}
void moveDoor()
{

}
void posPlot()
{

}
void queueSystem()
{

}