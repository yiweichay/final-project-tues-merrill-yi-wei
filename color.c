#include <xc.h>
#include <math.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include "dc_motor.h"

extern struct DC_motor motorL, motorR;

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at Clear low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop();          //Stop condition
    return tmp;
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void){
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at GREEN low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop(); 
    return tmp;
}

unsigned int color_read_Blue(void){
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at BLUE low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop();          //Stop condition
    return tmp;
}

void read_colours(struct RGB_val *m){
    (m->C) = color_read_Clear();
    (m->R) = color_read_Red();
    (m->G) = color_read_Green();
    (m->B) = color_read_Blue();
}

unsigned int isbtw(float num, float low, float high){
    if (num>=low && num<=high){return 1;}
    else {return 0;}
}

void calibrateW(struct RGB_val *m){
    (m->whiteR) = color_read_Red();
    (m->whiteG) = color_read_Green();
    (m->whiteB) = color_read_Blue();
}

void calibrateB(struct RGB_val *m){
    (m->blackR) = color_read_Red();
    (m->blackG) = color_read_Green();
    (m->blackB) = color_read_Blue();
}

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
    
    // Values from 
    
    // Red - will output 0 (Good for 1)
    if (isbtw(RelR,5.1,20.5)==1 && isbtw(RelG,2.2,3.8)==1 && isbtw(RelB,1.8,5.5)==1 && lumin>800)
    {out = 0;} 
    
    // Green - will output 1 
    if (isbtw(RelR,0.2,0.49)==1 && isbtw(RelG,0.28,0.59)==1 && isbtw(RelB,0.7,0.9)==1 && lumin>835)
    {out = 1;} 
    
    // Blue - will output 2 (Good for 1)
    if (isbtw(RelR,0.1,0.55)==1 && isbtw(RelG,0.1,0.39)==1 && isbtw(RelB,1.0,1.2)==1 && lumin>800)
    {out = 2;}
    
    // Yellow - will output 3 
    if (isbtw(RelR,1.2,1.4)==1 && isbtw(RelG,1.5,1.69)==1 && isbtw(RelB,0.8,0.9)==1 && lumin>850)
    {out = 3;}
   
     // Pink - will output 4
    if (isbtw(RelR,1.56,1.83)==1 && isbtw(RelG,1.23,1.50)==1 && isbtw(RelB,1.15,1.3)==1 && lumin>830)
    {out = 4;}
   
     // Orange - will output 5
    if (isbtw(RelR,3.1,4.85)==1 && isbtw(RelG,2.2,2.83)==1 && isbtw(RelB,1.27,1.8)==1 && lumin>820)
    {out = 5;}
    
     // Light Blue - will output 6 
    if (isbtw(RelR,0.6,0.86)==1 && isbtw(RelG,0.6,0.85)==1 && isbtw(RelB,0.95,1.12)==1 && lumin>820) 
    {out = 6;}
    
     // White - will output 7 might need to change to raw values PROBLEMATIC
    if (isbtw(RelR,0.9,1.1)==1 && isbtw(RelG,0.8,1.0)==1 && isbtw(RelB,0.95,1.1)==1 && lumin>890)
    {out = 7;}
    
     // Black - will output 8 (FOR RIGHT IS WEAK)
    if (RedRatio < 50 && GreenRatio < 50 && BlueRatio < 50) {out = 8;}
    
    return out;    
}

void White(struct DC_motor *mL, struct DC_motor *mR,unsigned int movementArray[], unsigned int movements,unsigned int timerArray[])
{
    stop(mL, mR);
    turnRight180(mL, mR);
    __delay_ms(1000);
    for (int i=0; i<movements; i++){
        if (movementArray[movements-i-1] == 0){turnLeft90(mL, mR);}
        else if (movementArray[movements-i-1] == 1){turnRight90(mL, mR);}
        else if (movementArray[movements-i-1] == 2){turnRight180(mL, mR);}
        else if (movementArray[movements-i-1] == 3){reverseTurnLeft90(mL, mR);}
        else if (movementArray[movements-i-1] == 4){reverseTurnRight90(mL, mR);}
        else if (movementArray[movements-i-1] == 5){turnLeft135(mL, mR);}
        else if (movementArray[movements-i-1] == 6){turnRight135(mL, mR);}
        else if (movementArray[movements-i-1] == 9){stop(mL, mR);}
        unsigned int tempTimerVal = 0;
        forward(mL, mR);
        TMR0H = 0;
        TMR0L = 0;
        while(tempTimerVal < timerArray[movements-i-1]){
            tempTimerVal = TMR0L;
            tempTimerVal |= (TMR0H << 8);
        }
    }
    stop(mL, mR);
}

//function to keep track of time taken for each movement 
void updateMovementCount(unsigned int movementCode,unsigned int movementArray[], unsigned int movements,unsigned int timerArray[])
{
    unsigned int tempTimerVal = TMR0L;
    tempTimerVal |= (TMR0H << 8);
    timerArray[movements] = tempTimerVal;
    movementArray[movements] = movementCode;
    TMR0H = 0;
    TMR0L = 0;
}

