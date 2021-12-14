# Course project - Mine navigation search and rescue

## Challenge brief

Your task is to develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Your robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

From these objectives and the "Search and Rescue" context of the project, we formulated certain conditions that the buggy had to adhere to which includes:
- Buggy must not impact the colour card when identifying 
- Buggy should continue to use its tires for traction in the 'mine' 
- To ensure proper distancing from the colour card after sensing, the buggy will reverse momentarily before executing appropriate action
- To store and execute the retracing of the path through using its memory

## Initial Project Planning and Working with GitHub

At the start of the project, we created an excel sheet and logged in all the critical elements of this project. This is so that both of us are aware of the parts we are working on and hence will not have overlapping work done. An example of this activity log is shown below.

![Excel log](https://user-images.githubusercontent.com/92339387/145869671-09498700-2b12-47b2-816b-d9fdbcad3e76.jpg)


During the project, we made use of branches to test out different codes before merging them into the main file. This is also shown below:

![Branches](https://user-images.githubusercontent.com/92339387/145869680-b3c656d9-d803-45c0-901b-5a8a498d3408.jpg)

## Code Management 

It was decided that for clarity sake, processes and functions that were essential to the functioning of the buggy would be written in main.c, while peripheral helper functions such as the associated motor movements and colour identification process would be written outside of main.c. We believe this would aid in the readability and compartmentalisation of our code, as well as reduce the number of arguments needed for a particular function.

Script  | Description
------------- | -------------
main.c  | Any functions, actions and programming that is essential to the running of the buggy through the maze
color.c  | Colour Identification Function and Memory Playback Execution Function 
dc_motor.c  | Associated actions required for each colour
interrupts.c  | For exception handling
timer.c  | Timer initialisation for timing of each movement and exception handling
i2c.c  | For communication with RGB Sensor (ColorClick)
serial.c  | For development, data acquistion and debugging purposes

## Guide to set up the buggy

1. Place buggy at the starting point in the respective "mine". When the buggy is ready for calibration, LED D7 turns on. A white card and black card is needed for the initial calibration. 
2. Place the white card directly infront of the light sensor and toggle button RF2. LED D7 will flash once to indicate that the white card has been calibrated.
3. Place the black card approximately 4cm away from the light sensor for calibration. This is to account for ambient lighting and reflectivity of the black card. Toggle button RF2 again to calibrate the black card. LED D7 will flash once again. Once the calibration process is complete, LED D7 turns off while LED H3 turns on. This means that the buggy is ready to start the colour sensing process.
4. To start the buggy, toggle button RF3.

A video which demonstrates the setting up and initial calibration process of the buggy is included [here](https://imperiallondon-my.sharepoint.com/:v:/g/personal/ywc19_ic_ac_uk/EVad5Vj5tUxGnKjwEe3ywaEBDxlz3bQ9RxnZfVTA16CrmQ?e=TUmPaU).

## Code Development 
### Environment Calibration and Colour Sensing

After trialing and testing through 3 different methods of colour identification (M1:Raw Values, M2:Hue Saturation Value, M3:RGB Proportion of Clear), we settled on the principle used in our colour sensing process is the relative proportions of Red, Green and Blue (RGB) to one another, more specifically, the ratios R:G, R:B and B:G which stay relatively consistent for each colour card. A common false positive situation identified was the false identification of Orange from a distant Red card, likewise for Light Blue and White. To combat this, a fourth condition is implemented where the "Perceived Brightness" or relative luminance from the LED reflecting on the card must meet a certain threshold. This luminance threshold also differs for each card colour. Hence, these ratios create 4 conditions that the current RGB sensor must meet before identifying a colour, thereby making the process robust. 

In order to get these values to be consistent, the calibration function at the beginning accounts for the ambient surrounding light by registering the maximum and minimum RGB values with the white card and slightly distanced black card accordingly.  

```
unsigned int determine_color_new(struct RGB_val *m){         
    unsigned int RedRatio, GreenRatio, BlueRatio;
    float RelR, RelG, RelB;
    unsigned int out = 9;
    
    // Perceived Brightness
    // Must be a certain threshold to ensure object is in front and not falsely identifying colour from a distance 
    unsigned int lumin = (0.2126*(m->R)) + (0.7152*(m->G)) + (0.0722*(m->B));
    
    // White ratio would be 1 for everything, Multiplied by 10,000 to increase dynamic range 
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
    .
    . // See color.c file for rest of conditions for other card colours  
    .
    return out;  
}
```

The thresholds for each card colour were determined by adjusting the card at varying distances from the RGB sensors and using serial.c to read the values.  

### Motor Movements 

The associated movements for each of the colours were programmed in dc_motor.c. It was decided that a momentary reverse action was appropriate to get the buggy centred in the middle of the maze. As an example, the action associate with Blue is shown below. 

```
//function to turn robot 180 to the right (Blue)
void turnRight180(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL, mR);
    __delay_ms(50);
    reverse(mL, mR);
    __delay_ms(400);
    stop(mL, mR);
    __delay_ms(150);
    
    (*mL).direction = 1; //0 means reverse direction
    (*mR).direction = 0;
    for (unsigned int i = 0; i < 75; ++i){ // Dont want too much power
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    __delay_ms(400); //Adjust Timing 
    stop(mL, mR);
    __delay_ms(50);
}
```

### Memory Storage and Playback

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

### Exception Handling 

An interrupt was used to handle the exception of bringing the buggy back "home" when it loses its way. A timer prescaler of 1:256 was initialised. This meant that the timer will overflow every approximately 16 seconds. Hence, if the buggy does not detect a colour card within 16 seconds, it will execute the White() function which will retrace its path and bring the buggy back to home. The interrupt function is shown as follows:

```
//initialise int for interrupt 
static volatile unsigned int maxTimeElapsed = 0;

//Interrupt to handle exception of retracing path if no card is detected within approx 16 secs
void __interrupt(high_priority) HighISR() // If overrun by 16s, will then trigger memory 
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if(PIR0bits.TMR0IF){
        maxTimeElapsed = 1;
        TMR0H = 0;
        TMR0L = 0;
        PIR0bits.TMR0IF = 0; // clearing the flag
    } 
}
```


## What could be improved

While we strived to achieve and refine the main objectives of the project which largely include the colour identification and memory playback functionalities, this resulted in less attention being paid to certain aspects of the project. If time permits, we would have strived to further improve: 

1. The calibration and preciseness of the motor actions for the associated colours 
2. The correction of the slight deviation from a straight line path when moving forward
