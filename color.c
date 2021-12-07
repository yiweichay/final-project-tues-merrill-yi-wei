#include <xc.h>
#include <math.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include "dc_motor.h"

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

unsigned int determine_color1(struct RGB_val *m){
    unsigned int data[9][3] = { // White test
        {47,1390,775}, // 0: Red - problem
        {2385,1660,795}, // 1: Green
        {1670,1162,599}, // 2: Blue
        {2671,1473,737}, // 3: Yellow - problem
        {2853,1391,787}, // 4: Pink
        {2422,991,564}, // 5: Orange - problem
        {2225,1507,788}, // 6: Light Blue
        {2702,1654,888}, // 7: White
        {47,23,18}   // 8: Black
    };
         
    unsigned int temp = 1000;
    unsigned int out;
    
    for (unsigned int j=0; j<9; j++) {
        unsigned int rr = (data[j][0]-(m->R))*(data[j][0]-(m->R));
        unsigned int gg = (data[j][1]-(m->G))*(data[j][1]-(m->G));
        unsigned int bb = (data[j][2]-(m->B))*(data[j][2]-(m->B));
        unsigned int calc = sqrt(rr + gg + bb);
        if (calc < temp) { // get the closest value
            temp = calc; 
            if (calc < 30){out = j;} // threshold similarity
            else {out = 9;} // means ambient 
        }
    }
    return out;
}

unsigned int determine_color2(struct RGB_val *m){
    unsigned int data[9][3] = { // Red test
        {2449,158,300}, // 0: Red - problem
        {1737,168,235}, // 1: Green
        {1692,147,239}, // 2: Blue
        {2544,250,334}, // 3: Yellow - problem
        {2414,237,248}, // 4: Pink
        {2553,177,316}, // 5: Orange - problem
        {2221,233,335}, // 6: Light Blue
        {2576,278,391}, // 7: White
        {818,382,193}   // 8: Black
    };
         
    unsigned int temp = 1000;
    unsigned int out;
    
    for (unsigned int j=0; j<9; j++) {
        unsigned int rr = (data[j][0]-(m->R))*(data[j][0]-(m->R));
        unsigned int gg = (data[j][1]-(m->G))*(data[j][1]-(m->G));
        unsigned int bb = (data[j][2]-(m->B))*(data[j][2]-(m->B));
        unsigned int calc = sqrt(rr + gg + bb);
        if (calc < temp) { // get the closest value
            temp = calc; 
            if (calc < 30){out = j;} // threshold similarity
            else {out = 9;} // means ambient 
        }
    }
    return out;
}

unsigned int determine_color3(struct RGB_val *m){
    unsigned int data[9][3] = { // Red test
        {415,901,344}, // 0: Red - problem
        {299,1129,426}, // 1: Green
        {256,1107,430}, // 2: Blue
        {515,1383,501}, // 3: Yellow - problem
        {475,1226,481}, // 4: Pink
        {445,997,377}, // 5: Orange - problem
        {415,1417,559}, // 6: Light Blue
        {535,1580,633}, // 7: White
        {1177,244,183}   // 8: Black
    };
         
    unsigned int temp = 1000;
    unsigned int out;
    
    for (unsigned int j=0; j<9; j++) {
        unsigned int rr = (data[j][0]-(m->R))*(data[j][0]-(m->R));
        unsigned int gg = (data[j][1]-(m->G))*(data[j][1]-(m->G));
        unsigned int bb = (data[j][2]-(m->B))*(data[j][2]-(m->B));
        unsigned int calc = sqrt(rr + gg + bb);
        if (calc < temp) { // get the closest value
            temp = calc; 
            if (calc < 30){out = j;} // threshold similarity
            else {out = 9;} // means ambient 
        }
    }
    return out;
}

unsigned int isbtw(float num, float low, float high){
    if (num>=low && num<=high){return 1;}
    else {return 0;}
}

void calibrate(struct RGB_val *m){
    (m->ambientC) = color_read_Clear();
    (m->ambientR) = color_read_Red();
    (m->ambientG) = color_read_Green();
    (m->ambientB) = color_read_Blue();
}

unsigned int determine_color_new(struct RGB_val *m){         
    float RedRatio, GreenRatio, BlueRatio;
    unsigned int out = 9;
    
    // White ratio would be 1 for everything 
    RedRatio = (float)m->R / (float)m->C; 
    GreenRatio = (float)m->G / (float)m->C;
    BlueRatio = (float)m->B / (float)m->C;
    // Normalise against white? but what for 
    
    // Red - will output 0 
    if (isbtw(RedRatio,0.795,0.870)==1 && isbtw(GreenRatio,0.140,0.169)==1 && isbtw(BlueRatio,0.185,0.210)==1)
    {out = 0;} 
    
    // Green - will output 1 
    if (isbtw(RedRatio,0.450,0.525)==1 && isbtw(GreenRatio,0.325,0.380)==1 && isbtw(BlueRatio,0.240,0.270)==1)
    {out = 1;} 
    
    // Blue - will output 2 
    if (isbtw(RedRatio,0.400,0.479)==1 && isbtw(GreenRatio,0.305,0.340)==1 && isbtw(BlueRatio,0.305,0.345)==1)
    {out = 2;}
    
    // Yellow - will output 3 
    if (isbtw(RedRatio,0.645,0.680)==1 && isbtw(GreenRatio,0.250,0.275)==1 && isbtw(BlueRatio,0.175,0.190)==1)
    {out = 3;}
   
     // Pink - will output 4
    if (isbtw(RedRatio,0.650,0.679)==1 && isbtw(GreenRatio,0.220,0.235)==1 && isbtw(BlueRatio,0.215,0.235)==1)
    {out = 4;}
    
     // Orange - will output 5
    if (isbtw(RedRatio,0.795,0.820)==1 && isbtw(GreenRatio,0.165,0.183)==1 && isbtw(BlueRatio,0.175,0.191)==1)
    {out = 5;}
    
     // Light Blue - will output 6 
    if (isbtw(RedRatio,0.500,0.550)==1 && isbtw(GreenRatio,0.285,0.310)==1 && isbtw(BlueRatio,0.259,0.280)==1)
    {out = 6;}
    
     // White - will output 7
    if (isbtw(RedRatio,0.565,0.605)==1 && isbtw(GreenRatio,0.259,0.285)==1 && isbtw(BlueRatio,0.238,0.255)==1)
    {out = 7;}
    
     // Black - will output 8 (FOR RIGHT IS WEAK)
    if (isbtw(RedRatio,0.581,0.606)==1 && isbtw(GreenRatio,0.240,0.276)==1 && isbtw(BlueRatio,0.200,0.245)==1)
    {out = 8;}
    
    // Bottom R: 570-590 G: 0.250-0.270 B: 0.210-230
    // Right R: G: B:
    // Top R: G: B:
    // Left R: G: B: 
    
    return out;    
}

// Red: R: 0.700,0.800      G: 0.180,0.210      B:0.180,0.199
// Green: R: 0.700,0.800      G: 0.313,0.335      B:0.220,0.240
// Blue: R: 0.410,0.460      G: 0.180,0.210      B:0.180,0.199
// Yellow: R: 0.610,0.625      G: 0.265,0.279     B:0.160,0.176
// Pink: R: 0.625,0.646      G: 0.230,0.240      B:0.200,0.209
// Orange: R: 0.715,0.755      G: 0.185,0.215      B:0.172,0.181
// Light Blue: R: 0.485,0.502      G: 0.305,0.315      B:0.245,0.257
// White: R: 0.545,0.555      G: 0.280,0.286      B:0.225,0.230
// Black: R: 0.548,0.560      G: 0.265,0.279      B:0.202,0.212