#include <xc.h>
#include "dc_motor.h"

// function initialise T2 and PWM for DC motor control
void initDCmotorsPWM(int PWMperiod){
	//initialise your TRIS and LAT registers for PWM
    TRISEbits.TRISE2=0; //output on RE2
    TRISCbits.TRISC7=0; //output on RC7
    TRISEbits.TRISE4=0;
    TRISGbits.TRISG6=0;
    LATEbits.LATE2=0; // 0 output on RB0
    LATCbits.LATC7=0; // 0 output on RB2
    LATEbits.LATE4=0;
    LATGbits.LATG6=0;

    // timer 2 config
    T2CONbits.CKPS= 0b100; // 1:16 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    T2PR= 99; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    RE2PPS=0x0A; //PWM6 on RE2
    RC7PPS=0x0B; //PMW7 on RC7

    PWM6DCH=0; //0% power
    PWM7DCH=0; //0% power
    
    PWM6CONbits.EN = 1; //enable PWM generation
    PWM7CONbits.EN = 1; //enable PWM generation
}


// function to set PWM output from the values in the motor structure
void setMotorPWM(struct DC_motor *m)
{
	int PWMduty; //tmp variable to store PWM duty cycle

	if (m->direction){ //if forward
		// low time increases with power
		PWMduty=m->PWMperiod - ((int)(m->power)*(m->PWMperiod))/100;
	}
	else { //if reverse
		// high time increases with power
		PWMduty=((int)(m->power)*(m->PWMperiod))/100;
	}

	*(m->dutyHighByte) = PWMduty; //set high duty cycle byte
        
	if (m->direction){ // if direction is high
		*(m->dir_LAT) = *(m->dir_LAT) | (1<<(m->dir_pin)); // set dir_pin bit in LAT to high without changing other bits
	} 
    else {
		*(m->dir_LAT) = *(m->dir_LAT) & (~(1<<(m->dir_pin))); // set dir_pin bit in LAT to low without changing other bits
	}
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL, struct DC_motor *mR)
{   
    while ((mL->power)>0 && (mR->power)>0){
        if ((mL->power) > 0){
            (mL->power) -= 1;
        }
        if ((mR->power) > 0){
            (mR->power) -= 1;
        }
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    } 
}

void reverse(struct DC_motor *mL, struct DC_motor *mR)
{
    (*mL).direction = 0;
    (*mR).direction = 0;
    (*mL).power = 50; // 0 means full speed
    (*mR).power = 50;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot turn left (Green)
void turnLeft90(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL, mR);
    __delay_ms(50);
    reverse(mL, mR);
    __delay_ms(400);
    stop(mL, mR);
    __delay_ms(150);
    
    (*mL).direction = 0; //0 means reverse direction
    (*mR).direction = 1;
    for (unsigned int i = 0; i < 75; ++i){ // Dont want too much power
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    __delay_ms(190); //Adjust Timing
    stop(mL, mR);
    __delay_ms(50);
}

//function to make the robot turn left 135 (Light Blue)
void turnLeft135(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL, mR);
    __delay_ms(50);
    reverse(mL, mR);
    __delay_ms(400);
    stop(mL, mR);
    __delay_ms(150);
    
    (*mL).direction = 0; //0 means reverse direction
    (*mR).direction = 1;
    for (unsigned int i = 0; i < 75; ++i){ // Dont want too much power
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    __delay_ms(255); //Adjust Timing
    stop(mL, mR);
    __delay_ms(50);
}

//function to make the robot turn right (Red)
void turnRight90(struct DC_motor *mL, struct DC_motor *mR)
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
    __delay_ms(205); //Adjust Timing
    stop(mL, mR);
    __delay_ms(50);
}

//function to make the robot turn right (Orange)
void turnRight135(struct DC_motor *mL, struct DC_motor *mR)
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
        __delay_us(50); //Adjust Timing
    }
    __delay_ms(270);
    stop(mL, mR);
    __delay_ms(50);
}

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

//function to make the robot go forward 
void forward(struct DC_motor *mL, struct DC_motor *mR)
{
    (*mL).direction = 1;
    (*mR).direction = 1;
    (*mL).power = 13; // 0 means full speed
    (*mR).power = 13;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make robot reverse 1 square and turn right 90 degrees
void reverseTurnRight90(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL, mR);
    __delay_ms(50);
    (*mL).direction = 0;
    (*mR).direction = 0;
    (*mL).power = 60; 
    (*mR).power = 60;
    setMotorPWM(mL);
    setMotorPWM(mR);
    __delay_ms(400); //Adjust Timing 
    stop(mL, mR);
    __delay_ms(300);
    turnRight90(mL, mR);
    __delay_ms(100);
}

//function to make robot reverse 1 square and turn left 90 degrees
void reverseTurnLeft90(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL, mR);
    __delay_ms(50);
    (*mL).direction = 0;
    (*mR).direction = 0;
    (*mL).power = 60;
    (*mR).power = 60;
    setMotorPWM(mL);
    setMotorPWM(mR);
    __delay_ms(400); //Adjust Timing
    stop(mL, mR);
    __delay_ms(300);
    turnLeft90(mL, mR);
    __delay_ms(100);
}