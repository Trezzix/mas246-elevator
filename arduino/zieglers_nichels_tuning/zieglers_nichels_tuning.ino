int pwmMax = 174;

const int enable = 7;
const int phase = 6;
const int decay = 5;

volatile boolean A_set = true;
volatile boolean B_set = true;
const byte encoderPinA = 20;
const byte encoderPinB = 21;
volatile long encoderPos = 0;
const float cps(32.0 * 131.0); //float since only used for maths, cps * gear ratio of 131:1

const float meterPerRot = 1; //m/rotation
const float maxRPS = (11500.0/131.0)/60.0; //rps of weight at 255 PWM, 

unsigned long timeAccStart;
float heightMoved = 0.0;
float speed = 0;
float heightMovedPrev = 0;
float tx = 0;
float tx_prev = 0;
float dt = 0;
bool a = true;
bool endingMove = false;
unsigned long finTime;

void setup() {
  Serial.begin(115200);
  delay(2000);
  servoEncoderInit();
  digitalWrite(phase,false);
  pinMode(enable,OUTPUT);
  pinMode(phase,OUTPUT);
  pinMode(decay,OUTPUT);
  digitalWrite(decay,LOW); // sets to slow in decay mode, could be switched to bipolar for simpler PID use
  timeAccStart = millis();
  heightMovedPrev = 0;


  Serial.print(millis());
  Serial.print(",");
  Serial.println(speed,16);
  analogWrite(enable,pwmMax);
}

void loop() {
  if(a)
  {
  tx = (static_cast<float>(millis() - timeAccStart))/1000.0; //s
  dt = tx - tx_prev;
  heightMoved =((static_cast<float>( readServoPosition() ) / cps) * meterPerRot);
  speed = (heightMoved-heightMovedPrev)/dt;
  Serial.print(millis());
  Serial.print(",");
  Serial.println(speed,16);
  if (endingMove == false)
        {
          finTime = millis();
          endingMove = true;
        }
  if ((millis() - finTime) >= 4000)
        {
          a = false;
          analogWrite(enable,0);
        }s

  }
  heightMovedPrev = heightMoved;
  tx_prev = tx;
}

void servoEncoderInit() //initialize encoder
{
  pinMode(encoderPinA, INPUT_PULLUP); //set as input and enable pullup for sinking
  pinMode(encoderPinB, INPUT_PULLUP);

  //attach ISR
  attachInterrupt(digitalPinToInterrupt(encoderPinA),doSignalA,CHANGE);

  //initialize A and B set
  A_set = !digitalRead(encoderPinA);
  B_set = !digitalRead(encoderPinB);
}

long readServoPosition()
{
  noInterrupts(); //disable interrupts while checking value
  long a = encoderPos;
  interrupts();
  return a;
}

void doSignalA() //interrupt function
{
  //read input
  if (digitalRead(encoderPinB) == A_set) //if A and B are both on, then it is cw
  {
    encoderPos--; //cw, has 131:1 gear ratio also
  }
  else
  {
    encoderPos++; //ccw
  }
  A_set = !A_set;
}
