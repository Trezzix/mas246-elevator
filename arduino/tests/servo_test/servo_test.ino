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

enum moveType
  {
    accelerate,
    constMove,
    decelerate,
    PIDcontrol,
    stopMove //temporary
  };

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

//declarations
void servoInit();
void servoEncoderInit();
float writeServo(float motorSpeedPerc, int motorPrev, unsigned long dt);

//global vars
int currentFloor; //for visuals
float currentHeight;
bool a = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servoInit();
  servoEncoderInit();
  currentFloor = 2;
  currentHeight = currentFloor * servoVars::floorDist;
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (a)
  {
    int elevatormove123 = moveElevator(up,1);
    if (elevatormove123 == 1)
    {
      Serial.println("move completed");
    }
    
    float temp = static_cast<float>( readServoPosition() ) / sensorVars::cps;
    //Serial.print(" servo pos: ");
    //Serial.println( temp );
    a = false; //run once
  }
}
