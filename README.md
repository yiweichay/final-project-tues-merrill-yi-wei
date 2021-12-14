# Course project - Mine navigation search and rescue

## Challenge brief

Your task is to develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Your robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

## Initial Project Planning and Working with GitHub

At the start of the project, we created an excel sheet and logged in all the critical elements of this project. This is so that both of us are aware of the parts we are working on and hence will not have overlapping work done. An example of this activity log is shown below.

![Excel log](https://user-images.githubusercontent.com/92339387/145869671-09498700-2b12-47b2-816b-d9fdbcad3e76.jpg)


During the project, we made use of branches to test out different codes before merging them into the main file. This is also shown below:

![Branches](https://user-images.githubusercontent.com/92339387/145869680-b3c656d9-d803-45c0-901b-5a8a498d3408.jpg)

## Guide to set up the buggy

1. Place buggy at the starting point in the respective "mine". When the buggy is ready for calibration, LED D7 turns on. A white card and black card is needed for the initial calibration. 
2. Place the white card directly infront of the light sensor and toggle button RF2. LED D7 will flash once to indicate that the white card has been calibrated.
3. Place the black card approximately 4cm away from the light sensor for calibration. This is to account for ambient lighting and reflectivity of the black card. Toggle button RF2 again to calibrate the black card. LED D7 will flash once again. Once the calibration process is complete, LED D7 turns off while LED H3 turns on. This means that the buggy is ready to start the colour sensing process.
4. To start the buggy, toggle button RF3.

A video which demonstrates the setting up and initial calibration process of the buggy is included in the video created under Project Description.

## How we tested the code
### Colour Sensing

The principle used in our colour sensing process is the relative proportions of Red, Green and Blue (RGB) to one another, more specifically, the ratios R:G, R:B and B:G. These create 3 conditions that the current RGB sensor is reading, which stay relatively consistent for each colour card. In order to get these values to be consistent, the calibration function at the beginning accounts for the ambient surrounding light by registering the maximum and minimum RGB values with the wihte card di. 

```
unsigned int determine_color_new(struct RGB_val *m){         
    unsigned int RedRatio, GreenRatio, BlueRatio;
    float RelR, RelG, RelB;
    unsigned int out = 9;
    
    // Perceived Brightness
    // Must be a certain threshold to ensure object is in front and not falsely identifying colour from a distance 
    unsigned int lumin = (0.2126*(m->R)) + (0.7152*(m->G)) + (0.0722*(m->B));
    
    // White ratio would be 1 for everything 
    RedRatio = ((float)(m->R - m->blackR) / (float)(m->whiteR - m->blackR))*10000; 
    GreenRatio = ((float)(m->G - m->blackG) / (float)(m->whiteG - m->blackG))*10000;
    BlueRatio = ((float)(m->B - m->blackB) / (float)(m->whiteB - m->blackB))*10000;
    
    // Relative ratio of RGB colours against each other 
    RelR = (float)RedRatio / (float)GreenRatio;
    RelG = (float)RedRatio / (float)BlueRatio;
    RelB = (float)BlueRatio / (float)GreenRatio;
    
    // Captures Exception of negative values: too dark
    if (RelR < 0) {RelR = 0;}
    if (RelG < 0) {RelG = 0;}
    if (RelB < 0) {RelB = 0;}
    
    // Red - will output 0 (Good for 1)
    if (isbtw(RelR,5.1,20.5)==1 && isbtw(RelG,2.2,3.8)==1 && isbtw(RelB,1.8,5.5)==1 && lumin>800)
    {out = 0;} 
```


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

![MPLAB X Debugger](https://user-images.githubusercontent.com/92339387/145985135-283a7f94-2161-44ae-9669-d3a9676fc14d.jpg)

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

## What could be improved
