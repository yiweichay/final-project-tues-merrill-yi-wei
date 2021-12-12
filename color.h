#ifndef _color_H
#define _color_H

#include <xc.h>
#include <math.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 





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

//definition of RGB structure
struct RGB_val { 
	unsigned int R;
	unsigned int G;
	unsigned int B;
    unsigned int C;
};

void RGB_init (void);
unsigned int isbtw(float num, float low, float high);
void calibrateW(struct RGB_val *m);
void calibrateB(struct RGB_val *m);
unsigned int determine_color_new(struct RGB_val *m);
unsigned int lumin(struct RGB_val *m);
void White(struct DC_motor *mL, struct DC_motor *mR);
unsigned int updateMovementCount(int movementCode);

#endif
