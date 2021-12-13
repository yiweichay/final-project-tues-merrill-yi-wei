# Course project - Mine navigation search and rescue

## Challenge brief

Your task is to develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Your robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

## Project Description


## Guide to set up the buggy

1. Place buggy at the starting point in the respective "mine". When the buggy is ready for calibration, LED D7 turns on. A white card and black card is needed for the initial calibration. 
2. Place the white card directly infront of the light sensor and toggle button RF2. LED D7 will flash once to indicate that the white card has been calibrated.
3. Place the black card approximately 4cm away from the light sensor for calibration. This is to account for ambient lighting and reflectivity of the black card. Toggle button RF2 again to calibrate the black card. LED D7 will flash once again. Once the calibration process is complete, LED D7 turns off while LED H3 turns on. This means that the buggy is ready to start the colour sensing process.
4. To start the buggy, toggle button RF3.

## How we tested the code
### Colour Sensing

### Memory storing and playback

One of the requirements of this project was to ensure that the buggy returns back to "home" when it reaches the last card. The last card in this case, is a White card. 2 functions were created for this sole purpose - one to update the fixed intialised arrays and one to execute the retracing of the path. 

We first tested the White function below to ensure that the it works. This is done by initialising a fixed array with specified elements. The code is uploaded onto the buggy and we checked to see if the buggy executes the movements stored in the movementArray accurately.

```
void White(struct DC_motor *mL, struct DC_motor *mR,unsigned int movementArray[], unsigned int movements,unsigned int timerArray[])
{
    //stop the buggy and turn 180 degrees
    stop(mL, mR);
    turnRight180(mL, mR);
    __delay_ms(1000);
    //a for loop is used to carry out the movements in reverse
    for (int i=0; i<movements; i++){
        if (movementArray[movements-i-1] == 0){turnLeft90(mL, mR);}
        else if (movementArray[movements-i-1] == 1){turnRight90(mL, mR);}
        else if (movementArray[movements-i-1] == 2){turnRight180(mL, mR);}
        else if (movementArray[movements-i-1] == 3){reverseTurnLeft90(mL, mR);}
        else if (movementArray[movements-i-1] == 4){reverseTurnRight90(mL, mR);}
        else if (movementArray[movements-i-1] == 5){turnLeft135(mL, mR);}
        else if (movementArray[movements-i-1] == 6){turnRight135(mL, mR);}
        else if (movementArray[movements-i-1] == 9){stop(mL, mR);}
        //after each movement, function reads the stored timer count
        //forward function is carried out for the specified timer element
        unsigned int tempTimerVal = 0;
        forward(mL, mR);
        TMR0H = 0;
        TMR0L = 0;
        while(tempTimerVal < timerArray[movements-i-1]){
            tempTimerVal = TMR0L;
            tempTimerVal |= (TMR0H << 8);
        }
    }
    //stop the buggy after the retracing path is done
    stop(mL, mR);
}
```

The updateMovementCount() function below fills the timerArray and movementArray after each colour card is read during its drive through the "mine". To ensure that the elements filling up the arrays are correct, the MPLAB X debugger was used. A break point was created in the main file right after where the function was called and the variables in the respective arrays could be seen throught the variable terminal as shown below. 

![MPLAB X Debugger](

```
void updateMovementCount(unsigned int movementCode,unsigned int movementArray[], unsigned int movements,unsigned int timerArray[])
{
    //to return the full 16-bit value
    unsigned int tempTimerVal = TMR0L;
    tempTimerVal |= (TMR0H << 8);
    //the time recorded between each card is stored in timerArray
    timerArray[movements] = tempTimerVal;
    //the movement code (0-8) is stored in movementArray
    movementArray[movements] = movementCode;
    //timer resets
    TMR0H = 0;
    TMR0L = 0;
}
```

## How to improve
