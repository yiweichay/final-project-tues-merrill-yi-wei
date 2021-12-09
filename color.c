#include <xc.h>

#include "color.h"
#include "i2c.h"
#include "dc_motor.h"


extern struct DC_motor motorL, motorR;
//to test the code
//int movements = 4;
//int timerArray[] = {400, 1024, 512, 256};
//int movementArray[] = {1, 9, 0, 2};
static volatile int movements = 0;
int timerArray[] = {};
int movementArray[] = {};

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

unsigned int color_read_Clear(void){
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

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

// initialise the LED
void RGB_init (void)
{
    LATGbits.LATG1=1;   //set initial output state
    LATAbits.LATA4=1;
    LATFbits.LATF7=1;
    TRISGbits.TRISG1=0; //set TRIS value for pin (output)
    TRISAbits.TRISA4=0;
    TRISFbits.TRISF7=0;
}

float determine_color_new(struct RGB_val *m){         
    unsigned int temp = 1000;
    unsigned int out;
    float RedRatio, GreenRatio, BlueRatio, Hue;
    float volatile RatioMax, RatioMin, Saturation;
    
    RedRatio = ((float)(m->R) / (float)(m->C));
    GreenRatio = ((float)(m->G) / (float)(m->C));
    BlueRatio = ((float)(m->B) / (float)(m->C));
    
    RatioMax = MAX_FLOAT(MAX_FLOAT(RedRatio, GreenRatio), BlueRatio);
    RatioMin = MIN_FLOAT(MIN_FLOAT(RedRatio, GreenRatio), BlueRatio);
    
    if (RatioMax > 0)
        Saturation = (RatioMax - RatioMin) / RatioMax;
    else
        Saturation = 0;

    if (Saturation == 0)
        Hue = 0;
    else 
    {
        if (RatioMax == RedRatio)
            Hue = (GreenRatio - BlueRatio) / (RatioMax - RatioMin);
        else if (RatioMax == GreenRatio)
            Hue = 2 + (BlueRatio - RedRatio) / (RatioMax - RatioMin);
        else
            Hue = 4 + (RedRatio - GreenRatio) / (RatioMax - RatioMin);

        Hue = Hue / 6;

        if (Hue < 0)
            Hue += 1;
    }
    
    return Hue; 
}

//these are the movementCodes
//0 = red
//1 = green
//2 = blue
//3 = yellow
//4 = pink
//5 = orange
//6 = light blue
//7 = white
//8 = black (not sure what to do with this yet)
//9 = ambient (move forward)

//for example, when it reaches black, movementArray[10]
//what value is in the array if there are more array sizes than movements?
//need to store the time taken for forward movement 

void Black(struct DC_motor *mL, struct DC_motor *mR)
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
        else if (movementArray[movements-i-1] == 9){forward(mL, mR);}
        int tempTimerVal = 0;
        TMR0H = 0;
        TMR0L = 0;
        while(tempTimerVal < timerArray[movements-i-1]){
            tempTimerVal = TMR0L;
            tempTimerVal += (TMR0H << 8);
        }
    }
    stop(mL, mR);
}


//function to keep track of time taken for each movement 
unsigned int updateMovementCount(int movementCode)
{
    int tempTimerVal = TMR0L;
    tempTimerVal += (TMR0H << 8);
    timerArray[movements] = tempTimerVal;
    movementArray[movements] = movementCode;
    movements++;
    TMR0H = 0;
    TMR0L = 0;
    //return timerArray[movements];
    //return movementArray;
}




