#ifndef _color_H
#define _color_H

// Find maximal floating point value
#define MAX_FLOAT(a, b) (((a) > (b)) ? (a) : (b))

// Find minimal floating point value
#define MIN_FLOAT(a, b) (((a) < (b)) ? (a) : (b))

#include <xc.h>
#include <math.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

struct RGB_val { 
        unsigned int C;
        unsigned int R;
        unsigned int G;
        unsigned int B;
        unsigned int t1r;
        unsigned int t1g;
        unsigned int t1b;
        unsigned int t2r;
        unsigned int t2g;
        unsigned int t2b;
        unsigned int t3r;
        unsigned int t3g;
        unsigned int t3b;
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
void read_color_sensor(struct RGB_val *m);
//unsigned int determine_color(struct RGB_val *m);
unsigned int determine_color1(struct RGB_val *m);
unsigned int determine_color2(struct RGB_val *m);
unsigned int determine_color3(struct RGB_val *m);
float determine_color_new(struct RGB_val *m);

#endif
