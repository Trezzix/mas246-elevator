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

const float meterPerRot = 1.0; //m/rotation
const float maxRPS = (11500.0/131.0)/60.0; //rps of weight at 255 PWM, 

unsigned long timeAccStart;
float heightMoved = 0.0;
float speed2 = 0.0;
float heightMovedPrev = 0;
float dt = 0;
bool a = true;
bool endingMove = false;
unsigned long finTime;
float averaged =0.0;
float movingaverage;
float heightMovedavg;
int i = 0;

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
  analogWrite(enable,pwmMax);
}

void loop() {
  if(a)
  {

  if ((millis() % 10) == 0)
  {
    heightMoved =((static_cast<float>( readServoPosition() ) / cps) * meterPerRot);
    dt = 10.0/1000.0;//s
    delayMicroseconds(10);
    speed2 = (heightMoved-heightMovedPrev)/dt;
    heightMovedPrev = heightMoved;
    movingaverage += speed2;
    if(i == 5)
    {
      averaged = movingaverage / 5.0;
      Serial.print(millis());
      Serial.print(",");
      Serial.print(heightMoved,8);
      Serial.print(",");
      Serial.print(movingaverage,8);
      Serial.print(",");
      Serial.print(speed2,32);
      Serial.print(",");
      Serial.println(averaged,8); //speed2 averaged
      
      i=0;
      movingaverage = 0.0;
    }
    i++;
  }
  
  if (endingMove == false)
        {
          finTime = millis();
          endingMove = true;
        }
  if ((millis() - finTime) >= 4000)
        {
          a = false;
          analogWrite(enable,0);
        }

  }
  /*
    for(int i = 0; i < (k-1); i++)
    {
      movingaverage[i] = movingaverage[i+1];
    }
    movingaverage[k] = heightMoved;
    averaged = 0.0;
    for(int i = 0; i < k; i++)
    {
      averaged += static_cast<float>(movingaverage[i]);
    }
    
    heightMovedavg = averaged/static_cast<float>(k);
    */
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
