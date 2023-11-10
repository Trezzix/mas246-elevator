enum elevatorDir
{
  elevUp,
  elevDown,
  reqInternal //internal request is independent of direction
};

enum elevatorState
  {
    first_Floor,
    idle,
    prepare_up,
    prepare_down,
    moving_algorithm,
   };

struct buttonPressType
{
  elevatorDir bRequestDir;
  int8_t floorNum;
};

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

namespace HMIvars
{
    int buttonNumber;
    int buttonPress = 0;
    bool buttonState;
    int ledNumber;
    bool ledState;
}

//global variables:
  int currentFloor;
  float currentHeight;
  int8_t elevatorRequestsCurrent[10];
  int8_t elevatorRequestsAlt[10];
  int gotoFloor;//temporary, change for future queue system
  unsigned long timeBegin = millis(); //for time keeping

  elevatorDir elevatorMoveDir;
//buttons

buttonPressType buttonRead(const int _buttonPress);
void checkButton();

void setup() {
  for(int i = 0; i < 10; i++)
  {
    elevatorRequestsCurrent[i] = -1;
    elevatorRequestsAlt[i] = -1;
  }
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);
  Serial.print("elevator moving up, current floor: ");
  currentFloor = 3;
  Serial.println(currentFloor);
  elevatorMoveDir = elevUp;

  currentHeight = static_cast<float>(currentFloor) * servoVars::floorDist;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("main requests: ");
  for(int i = 0;i < 10;i++)
  {
    Serial.print(elevatorRequestsCurrent[i]);
    Serial.print(" ");
  }
  Serial.println("");

  Serial.print("alt  requests: ");
  for(int i = 0;i < 10;i++)
  {
    Serial.print(elevatorRequestsAlt[i]);
    Serial.print(" ");
  }
  Serial.println("");
  clearRequest();
  checkButton();
}