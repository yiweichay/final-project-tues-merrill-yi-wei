#ifndef _color_H
#define _color_H

#include <xc.h>
#include <math.h>
#include "i2c.h"
#include "serial.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

struct RGB_val {   
        unsigned int blackR;  
        unsigned int blackG;
        unsigned int blackB;
        unsigned int whiteR;  
        unsigned int whiteG;
        unsigned int whiteB;
        unsigned int C;
        unsigned int R;
        unsigned int G;
        unsigned int B;
    };

/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Clear(void);
unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);
void read_colours(struct RGB_val *m);
unsigned int isbtw(float num, float low, float high);
void calibrateW(struct RGB_val *m);
void calibrateB(struct RGB_val *m);
unsigned int determine_color_new(struct RGB_val *m);
unsigned int lumin(struct RGB_val *m);

#endif
