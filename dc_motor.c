#include <xc.h>
#include "dc_motor.h"

// function initialise T2 and PWM for DC motor control
void initDCmotorsPWM(int PWMperiod){
	//initialise your TRIS and LAT registers for PWM
    TRISEbits.TRISE2=0; //output on RE2
    TRISCbits.TRISC7=0; //output on RC7
    TRISGbits.TRISG6=0; //output on RG6
    TRISEbits.TRISE4=0; //output on RE4
    LATEbits.LATE2=0; // 0 output on RE2
    LATCbits.LATC7=0; // 0 output on RC7
    LATGbits.LATG6=0; // 0 output on RG6
    LATEbits.LATE4=0; // 0 output on RE4

    // timer 2 config
    T2CONbits.CKPS=0b100; // 1:16 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    T2PR=99; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    RE2PPS=0x0A; //PWM6 on RE2
    RC7PPS=0x0B; //PMW7 on RC7

    PWM6DCH=0; //0% power
    PWM7DCH=0; //0% power
    
    PWM6CONbits.EN = 1;
    PWM7CONbits.EN = 1;
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
	} else {
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
        __delay_us(50);
    } 
}

//function to make the robot turn left 
void turnLeft(struct DC_motor *mL, struct DC_motor *mR)
{   
    (mL->direction) = 0;
    (mR->direction) = 1;
    setMotorPWM(mL);
    setMotorPWM(mR);
    for (unsigned int i = 0; i < 80; ++i){ // Dont want too much power
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
}

//function to make the robot turn right 
void turnRight(struct DC_motor *mL, struct DC_motor *mR)
{
    (mL->direction) = 1;
    (mR->direction) = 0;
    setMotorPWM(mL);
    setMotorPWM(mR);
    for (unsigned int i = 0; i < 80; ++i){
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
}

//function to make the robot go straight
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{   
    (mL->direction) = 1;
    (mR->direction) = 1;
    for (unsigned int i = 0; i < 90; ++i){
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
}

void turn90Left(struct DC_motor *mL, struct DC_motor *mR){
    turnLeft(mL,mR);
    __delay_us(60);
    stop(mL,mR);
}

void turn90Right(struct DC_motor *mL, struct DC_motor *mR){
    turnRight(mL,mR);
    __delay_us(60);
    stop(mL,mR);
}

void turn180Right(struct DC_motor *mL, struct DC_motor *mR){
    turnRight(mL,mR);
    __delay_ms(5);
    stop(mL,mR);
}

void clockwisesq(struct DC_motor *mL, struct DC_motor *mR){
    for (unsigned int i=0;i<4;++i){
        fullSpeedAhead(mL,mR);
        __delay_ms(10);
        stop(mL,mR);
        __delay_ms(15);   
        if (i<3){turn90Right(mL,mR);}
        __delay_ms(5);
    }
}

void anticlockwisesq(struct DC_motor *mL, struct DC_motor *mR){
    for (int i=0;i<4;++i){
        fullSpeedAhead(mL,mR);
        __delay_ms(10);
        stop(mL,mR);
        __delay_ms(15);   
        if (i<3) {turn90Left(mL,mR);}
        __delay_ms(5);
    }
}
