using namespace HMIvars;

enumFloors buttonRead(const int _buttonPress)
{
    //change later to Q = up floor 1, W
    switch (_buttonPress)
    {
        case '1':
        {
            return first;
            break;
        }
        case '2':
        {
            return second;
            break;
        }
        case '3':
        {
            return third;
            break;
        }
        case '4':
        {
            return fourth;
            break;
        }
        case 'q':
        {
            return firstUp;
            break;
        }
        case 'w':
        {
            return secondUp;
            break;
        }
        case 's':
        {
            return secondDown;
            break;
        }
        case 'e':
        {
            return thirdUp;
            break;
        }
        case 'd':
        {
            return thirdDown;
            break;
        }
        case 'f':
        {
            return fourthDown;
            break;
        }
        default:
        {
            return none;    
        }
    }
}

void ledWrite()
{
  
}
