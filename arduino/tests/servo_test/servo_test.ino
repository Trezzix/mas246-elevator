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

namespace servoVars
{
  int motorspeed = 64; //max 255
  int enable = 7;
  int phase = 6;
  int decay = 5;
  float elevatorAcc = 1.2; //m/s^2
  float elevatorSpeed = 1.0; //m/s
}

//declarations
void servoInit();

void setup() {
  // put your setup code here, to run once:
  servoInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
