//#include <LiquidCrystal.h>
using namespace HMIvars;

int checkButton()
{
  if (Serial.available() > 0) //char "10" = enter, can be ignored but will still create a serial input
    {
      // read the incoming byte:
      buttonPress = Serial.read();
      buttonPressType buttonPressed2; //this is made every input, a bit inefficient
      buttonPressed2 = buttonRead(buttonPress);

      if (buttonPressed2.floorNum != -1)
      {
          Serial.print("button pressed for floor: "); //print to serial
          Serial.print(buttonPressed2.floorNum);
          Serial.print(" with direction: ");
          Serial.println(buttonPressed2.bRequestDir);
          
          int a = queueSystem(buttonPressed2); //add button to queue system
          //write input:
          printElevList();
          ledWrite();//update LEDs
          return a; //return 0 if not added to queue
      }
    }
    return 0;//return 0 if no button pressed
}

buttonPressType buttonRead(const int _buttonPress)
{//returns struct depending on what keyboard button was pressed
  buttonPressType newButtonPress;
    switch (_buttonPress)
    {
        case '1':
        {
          newButtonPress.bRequestDir = reqInternal;
          newButtonPress.floorNum = 1;
          break;
        }
        case '2':
        {
          newButtonPress.bRequestDir = reqInternal;
          newButtonPress.floorNum = 2;
          break;
        }
        case '3':
        {
          newButtonPress.bRequestDir = reqInternal;
          newButtonPress.floorNum = 3;
          break;
        }
        case '4':
        {
          newButtonPress.bRequestDir = reqInternal;
          newButtonPress.floorNum = 4;
          break;
        }
        case 'q':
        {
          newButtonPress.bRequestDir = elevUp;
          newButtonPress.floorNum = 1;
          break;
        }
        case 'w':
        {
          newButtonPress.bRequestDir = elevUp;
          newButtonPress.floorNum = 2;
          break;
        }
        case 's':
        {
          newButtonPress.bRequestDir = elevDown;
          newButtonPress.floorNum = 2;
          break;
        }
        case 'e':
        {
          newButtonPress.bRequestDir = elevUp;
          newButtonPress.floorNum = 3;
          break;
        }
        case 'd':
        {
          newButtonPress.bRequestDir = elevDown;
          newButtonPress.floorNum = 3;
          break;
        }
        case 'f':
        {
          newButtonPress.bRequestDir = elevDown;
          newButtonPress.floorNum = 4;
          break;
        }
        default:
        {
          newButtonPress.bRequestDir = reqInternal;
          newButtonPress.floorNum = -1;
          break;   
        }
    }
    return newButtonPress;
}

int queueSystem(buttonPressType desiredFloor)
{//queue system, return 1 if new request changes current elevator move
  
  if (desiredFloor.bRequestDir == elevatorMoveDir || desiredFloor.bRequestDir == reqInternal) //checks if same direction
  {
    //put in main
    Serial.print("in direction, queing floor: ");
    Serial.println(desiredFloor.floorNum);
    switch (elevatorMoveDir)//checks direction
    {
      case elevUp:
      {
        if(currentHeight > (static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist)
        {//if requested button is in the same direction but elevator has to change direction to get there
          Serial.println("writing to alt 1");
          //write to alt queue
          createQueue(false,false,desiredFloor);
        }
        else if (((((static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist) > ((PIDvars::error) - 1.0))
                  || (((static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist) < ((PIDvars::error) + 1.0))) 
                  && (currentHeight < (static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist) )
        {//if within 1m of goal, to give PID time to slow down:
          lcdDisplay(0,"Moving up to: ",desiredFloor.floorNum);
          Serial.println("writing to main 2 (in move change)"); //correct if using pointers
          createQueue(true,true,desiredFloor);
          return 1;
          //write to main 
        }
        else
        {//button pressed too late and floor passed, writing to alternative queue
          Serial.println("writing to alt 5");
          createQueue(false,true,desiredFloor); 
          //write to alt 
        }
        break;
      }
      case elevDown: //does same as above but in opposite direction
      {
        if(currentHeight < (static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist)
        {
          Serial.println("writing to alt 3");
          //write to alt
          createQueue(false,false,desiredFloor);
        }
        else if (((((static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist) > ((PIDvars::error) - 1.0))
                  || (((static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist) < ((PIDvars::error) + 1.0))) 
                  && (currentHeight > (static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist) - servoVars::floorDist) )
        {
          lcdDisplay(0,"Moving dwn to: ",desiredFloor.floorNum);
          Serial.println("writing to main 4 (in move change)");
          createQueue(true,true,desiredFloor);
          return 1;
          //write to main 
        }
        else
        {
          Serial.println("writing to alt 6"); //correct if using pointers
          createQueue(false,true,desiredFloor);
          //write to alt 
        }
        break;
      }

    }
  }
  else //requested button is in the opposite direction, elevator has to change direction to get there
  {
    //put in alt
    Serial.print("not in direction, queing floor: ");
    Serial.println(desiredFloor.floorNum);
    switch(elevatorMoveDir)
    {
      case elevUp:
     {
      Serial.println("writing to alt 5");
      createQueue(false,false,desiredFloor); //correct when 4 - sd
      break;
     }
     case elevDown:
     {
      Serial.println("writing to alt 6");
      createQueue(false,false,desiredFloor); //correct when 1 - we
      break;
      }
    }
  }
  return 0;
}

void createQueue(bool mainList,bool dir,buttonPressType desiredFloor) //dir, true = up, false = down, queueing system
{//writes to queue
  int i = 0;
  bool a = true;
  bool sortUp;

  switch (elevatorMoveDir) //this calculation is already done, does it need to be done again here?
  {
     case elevUp:
     {
      sortUp = dir;
      break;
     }
    case elevDown:
    {
      sortUp = !dir;
      break;
    }
   }

  if (mainList) //could be a switch case,
  { //write to main requests (in same direction)
    while(a)//loop until completed, works as a manual For loop but loop can be stopped at any point
    {
      if ((
            ( (elevatorRequestsCurrent[i] > desiredFloor.floorNum || elevatorRequestsCurrent[i] == -1) && sortUp) 
            || ( (elevatorRequestsCurrent[i] < desiredFloor.floorNum) && !sortUp)
          ) 
          && (i < 10) ) //checks where to put newly requested floor depending on earlier requests
      {
        Serial.print("usable index found at i=");
        Serial.println(i);
        for (int o = 9; o > i;o--) //pushes all values up, will truncate last value but last value cannot exist because max button presses = 10
        {
          elevatorRequestsCurrent[o] = elevatorRequestsCurrent[o-1];
        }
        elevatorRequestsCurrent[i] = desiredFloor.floorNum;
        a = false;
      }
      else if ((i >= 10) || (elevatorRequestsCurrent[i] == desiredFloor.floorNum)) //if requested floor is already in requests
      {
        i = 0;
        Serial.println("error: button pressed before");
        a = false;
      }
      else
      {
        i++;
      }
    }
  }
  else
  {
    while(a) //does same as above but in opposite direction
    {
      if ((
            ( (elevatorRequestsAlt[i] > desiredFloor.floorNum || elevatorRequestsAlt[i] == -1) && sortUp) 
            || ( (elevatorRequestsAlt[i] < desiredFloor.floorNum) && !sortUp)
          ) 
          && (i < 10) ) //checks where to put newly requested floor depending on earlier requests
      {
        Serial.print("usable index found at i=");
        Serial.println(i);
        for (int o = 9; o > i;o--) //pushes all values up, will truncate last value but last value cannot exist because max button presses = 10
        {
          elevatorRequestsAlt[o] = elevatorRequestsAlt[o-1];
        }
        elevatorRequestsAlt[i] = desiredFloor.floorNum;
        a = false;
      }
      else if ((i >= 10) || (elevatorRequestsAlt[i] == desiredFloor.floorNum))
      {
        i = 0;
        Serial.println("error: button pressed before");
        a = false;
      }
      else
      {
        i++;
      }
    }
  }
}

void clearRequest() //checks if current floor is the same as the current request
{
  if(currentFloor == elevatorRequestsCurrent[0])
  {
    Serial.println("floor reached, truncating");
    for (int i = 0; i < 9;i++) //pushes all values down, truncating first
    {
      elevatorRequestsCurrent[i] = elevatorRequestsCurrent[i+1];
    }
    elevatorRequestsCurrent[9] = -1;
    printElevList();
  }
  else if((elevatorRequestsCurrent[0] == -1) && (elevatorRequestsAlt[0] != -1)) //all floor requests in same direction reached, switching to alt (opposite dir) if not empty
  {
    Serial.println("all requests completed, switching to alt");
    for (int i = 0; i < 10;i++) //pushes all values down, truncating first
    {
      elevatorRequestsCurrent[i] = elevatorRequestsAlt[i];
      elevatorRequestsAlt[i] = -1;
    }
  }
}

void printElevList() //print request list to serial port (slow)
{
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
}

void ledWrite() //turn on LEDS depending on requests
{
  uint8_t ledOn = 0; //not used as decimal number, each bit in the byte represents the state of one LED
  for(int i = 0; i < 10; i++) //go through both request lists
  {
    if (elevatorRequestsCurrent[i] != -1)
    {
      ledOn |= (1<<(elevatorRequestsCurrent[i]-1)); //set 1 to the relative bit if there is a request, uses bitshifts and boolean operations
    }
    if (elevatorRequestsAlt[i] != -1)
    {
      ledOn |= (1<<(elevatorRequestsAlt[i]-1));
    }
  }
  digitalWrite(led0,(ledOn & 1) == 1); //method below is not used for 1st LED
  digitalWrite(led1,((ledOn & (1<<1))>>1 ) == 1); //checks if the relative bit for the LED is set, then bitshifts to the right to 1 to compare if 1 or 0
  digitalWrite(led2,((ledOn & (1<<2))>>2 ) == 1); //if 1 == 1, returns true which turns on relative LED
  digitalWrite(led3,((ledOn & (1<<3))>>3 ) == 1);
}

void hmiInit() //initialize HMI
{
  //LEDs:
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  //lcd:
  pinMode(LCD_Backlight, OUTPUT);
  analogWrite(LCD_Backlight,128);
  lcd.begin(16,2);//set size of lcd
  lcd.clear();
}

//write to LCD screens: uses two functions depending on input
void lcdDisplay(const int y,const char lcdText[],const int floor) //y position, text (max 16 letters), floor number
{
  lcd.setCursor(0,y);
  lcd.print("                ");//clear the line...
  lcd.setCursor(0,y);
  lcd.print(lcdText);
  lcd.print(floor);
}

void lcdDisplay(const int y,const char lcdText[]) //y position and text, max 16 letters
{
  lcd.setCursor(0,y);
  lcd.print("                ");//clear the line...
  lcd.setCursor(0,y);
  lcd.print(lcdText);
}

