#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

struct DC_motor { //definition of DC_motor structure
    unsigned char power;         //motor power, out of 100
    unsigned char direction;     //motor direction, forward(1), reverse(0)
    unsigned char *dutyHighByte; //PWM duty high byte address
    unsigned char *dir_LAT; //LAT for dir pin
    unsigned char dir_pin; // pin number that controls direction on LAT
    unsigned int PWMperiod; //base period of PWM cycle
};

//function prototypes
void initDCmotorsPWM(int PWMperiod); // function to setup PWM
void setMotorPWM(struct DC_motor *m);
void stop(struct DC_motor *mL, struct DC_motor *mR);
void turnLeft90(struct DC_motor *mL, struct DC_motor *mR);
void turnLeft135(struct DC_motor *mL, struct DC_motor *mR);
void turnRight90(struct DC_motor *mL, struct DC_motor *mR);
void turnRight135(struct DC_motor *mL, struct DC_motor *mR);
void turnRight180(struct DC_motor *mL, struct DC_motor *mR);
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR);
void forward(struct DC_motor *mL, struct DC_motor *mR);
void reverse(struct DC_motor *mL, struct DC_motor *mR);
void reverseTurnRight90(struct DC_motor *mL, struct DC_motor *mR);
void reverseTurnLeft90(struct DC_motor *mL, struct DC_motor *mR);

#endif
