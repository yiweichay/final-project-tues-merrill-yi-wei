// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "dc_motor.h"
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include "interrupts.h"
#include "timers.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 

int movements = 0;
int timerArray[30] = {};
int movementArray[30] = {}; //define overall empty array

//void __interrupt(high_priority) HighISR()
//{
//    //add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
//    if(PIR0bits.TMR0IF){
//        sec++;
//        if (LATHbits.LATH3 = 1){//if reaches next card, turn on the LED RH3
//            timerCount++;
//            timerArray[timerCount] == sec; //timer element = current second
//            sec = 0;
//        }
//        PIR0bits.TMR0IF = 0; // clearing the flag
//    }
//}

void main(void){    
    
    initDCmotorsPWM(PWMcycle);
    initUSART4();
    Interrupts_init();
    Timer0_init(); //when button is pressed, then call TimerInit
    RGB_init();
    __delay_ms(500);
    color_click_init();
    
    int PWMcycle = 99;
    int movementArray[60] = {} //defining an array for the movement count
    //movementArray[count] = color number i.e 0-9]
    unsigned int count = 0;
    char colorRead;
    char string[20];
    
    struct RGB_val colorInput;
    struct DC_motor motorL, motorR; 		//declare two DC_motor structures 
    
    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.dutyHighByte=(unsigned char *)(&PWM6DCH);	//store address of PWM duty high byte
    motorL.dir_LAT=(unsigned char *)(&LATE); 		//store address of LAT
    motorL.dir_pin=4; 						//pin RE4 controls direction
    motorL.PWMperiod=PWMcycle; 			//store PWMperiod for motor

    //same for motorR but different PWM register, LAT and direction pin
    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.dutyHighByte=(unsigned char *)(&PWM7DCH);	//store address of PWM duty high byte
    motorR.dir_LAT=(unsigned char *)(&LATG); 		//store address of LAT
    motorR.dir_pin=6; 						//pin RG6 controls direction
    motorR.PWMperiod=PWMcycle;
    
    
    //initialise empty array/list (search about vectors)
    //timer to determine how long the car goes for
    // count ++
    //everytime it reaches a new card, store the instruction and timer value in an array and check timer value to see how long its been since previous instruction
    //reset timer
    //when reads black card/ wonders away -> turn 180 degrees and read perform the array
    //count --
    
    // setup pin for input (connected to left button)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin 
    
    LATDbits.LATD7 = 0;
    TRISDbits.TRISD7 =0;
    
    while(1){
        // send values to the serial to read the color of the cards
        // Receiving data
//        while (isDataInRxBuf()==1) {
//            colorRead = color_read_Red(); //change this, there should be some error here
//        }
        
//        colorInput.R = color_read_Red();
//        colorInput.G = color_read_Green();
//        colorInput.B = color_read_Blue();
//        sprintf(string,"%d, %d, %d//",colorInput.R,colorInput.G,colorInput.B);
        
        // If button pressed
        if (!PORTFbits.RF2) {
            colorInput.R = color_read_Red();
            colorInput.G = color_read_Green();
            colorInput.B = color_read_Blue();
            colorInput.C = color_read_Clear();
            float temp = determine_color_new(&colorInput);
            
            unsigned int int_part;
            unsigned int frac_part;
            int_part = temp/1;
            frac_part =(temp*1000)/1 - int_part*1000;
            
            sprintf(string,"%d.%03d",int_part, frac_part);
            TxBufferedString(string);
            sendTxBuf();
            //LATDbits.LATD7 = !LATDbits.LATD7 ;
            __delay_ms(200);
        }

    }
}