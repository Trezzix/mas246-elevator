//#include <LiquidCrystal.h>
using namespace HMIvars;

void checkButton()
{
  if (Serial.available() > 0) //char "10" = enter, can be ignored
    {
      // read the incoming byte:
      buttonPress = Serial.read();
      buttonPressType buttonPressed2; //this is made every loop, very inefficient, also rename
      buttonPressed2 = buttonRead(buttonPress);

      if (buttonPressed2.floorNum != -1)
      {
          Serial.print("button pressed for floor: ");
          Serial.print(buttonPressed2.floorNum);
          Serial.print(" with direction: ");
          Serial.println(buttonPressed2.bRequestDir);
          queueSystem(buttonPressed2);
          //write input:
          printElevList();
      }
    }
}

buttonPressType buttonRead(const int _buttonPress)
{
  buttonPressType newButtonPress;
    //change later to Q = up floor 1, W
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

void queueSystem(buttonPressType desiredFloor)
{
  
  if (desiredFloor.bRequestDir == elevatorMoveDir || desiredFloor.bRequestDir == reqInternal) //checks if same direction
  {
    //put in main
    Serial.print("in direction, queing floor: ");
    Serial.println(desiredFloor.floorNum);
    switch (elevatorMoveDir)
    {
      case elevUp:
      {
        if(currentHeight > (static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist))
        {
          Serial.println("writing to alt 1");
          //write to alt
          createQueue(false,false,desiredFloor);
        }
        else
        {
          Serial.println("writing to main 2"); //correct if using pointers
          createQueue(true,true,desiredFloor);
          //write to main 
        }
        break;
      }
      case elevDown:
      {
        if(currentHeight < (static_cast<float>(desiredFloor.floorNum) * servoVars::floorDist))
        {
          Serial.println("writing to alt 3");
          //write to alt
          createQueue(false,false,desiredFloor);
        }
        else
        {
          Serial.println("writing to main 4");
          createQueue(true,true,desiredFloor);
          //write to main 
        }
        break;
      }

    }
  }
  else
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
}

void createQueue(bool mainList,bool dir,buttonPressType desiredFloor)
{
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
  if (mainList)
  {
    while(a)
    {
      if ((
            ( (elevatorRequestsCurrent[i] > desiredFloor.floorNum || elevatorRequestsCurrent[i] == -1) && sortUp) 
            || ( (elevatorRequestsCurrent[i] < desiredFloor.floorNum) && !sortUp)
          ) 
          && (i < 10) ) //...
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
      else if ((i >= 10) || (elevatorRequestsCurrent[i] == desiredFloor.floorNum))
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
    while(a)
    {
      if ((
            ( (elevatorRequestsAlt[i] > desiredFloor.floorNum || elevatorRequestsAlt[i] == -1) && sortUp) 
            || ( (elevatorRequestsAlt[i] < desiredFloor.floorNum) && !sortUp)
          ) 
          && (i < 10) ) //...
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

void clearRequest()
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
  else if((elevatorRequestsCurrent[0] == -1) && (elevatorRequestsAlt[0] != -1)) //all floors in dir reached, switching to alt if not empty
  {
    Serial.println("all requests completed, switching to alt");
    for (int i = 0; i < 10;i++) //pushes all values down, truncating first
    {
      elevatorRequestsCurrent[i] = elevatorRequestsAlt[i];
      elevatorRequestsAlt[i] = -1;
    }
  }
}

void printElevList()
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

void ledWrite()
{
  
}

void lcdInit()
{
  pinMode(LCD_Backlight, OUTPUT);
  analogWrite(LCD_Backlight,128);
  lcd.begin(16,2);//set size of lcd
  lcd.clear();
}

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

