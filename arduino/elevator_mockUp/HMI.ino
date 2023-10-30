int buttonNumber;
bool buttonState;
int ledNumber;
bool ledState;

enum floors
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

floors buttonRead()
{
    //change later to Q = up floor 1, W
    if (serial.Read('1'))
    {
        return first;
    }
    else if (serial.Read('2'))
    {
        return second;
    }
    else if (serial.Read('3'))
    {
        return third;
    }
    else if (serial.Read('4'))
    {
        return fourth;
    }
    else if (serial.Read('Q')) // up, floor 1...
    {
        return firstUp;
    }
    else if (serial.Read('W')) // up, floor 2...
    {
        return secondUp;
    }
    else if (serial.Read('S')) // down, floor 2...
    {
        return secondDown;
    }
    else if (serial.Read('E')) // up, floor 3...
    {
        return thirdUp;
    }
    else if (serial.Read('D')) // down, floor 3...
    {
        return thirdDown;
    }
    else if (serial.Read('F')) // down, floor 4...
    {
        return fourthDown;
    }
    return none;
}
void ledWrite()
{

}