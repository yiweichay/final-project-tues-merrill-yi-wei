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

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    // Will probably need interrupts and timer scripts
    initDCmotorsPWM(99);
    color_click_init();
    initUSART4(); 
    Interrupts_init();

    char string[20];
    char string1[20];
    char string2[20];
    char string3[20];
    
    // setup pin for input (connected to left button)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    
    struct DC_motor motorL, motorR; 		//declare two DC_motor structures 
    unsigned int PWMcycle = 99;
    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.dutyHighByte=(unsigned char *)(&PWM6DCH);	//store address of PWM duty high byte
    motorL.dir_LAT=(unsigned char *)(&LATE); 		//store address of LAT
    motorL.dir_pin=4; 						//pin RE4 controls direction
    motorL.PWMperiod=PWMcycle; 			//store PWMperiod for motor
    
    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.dutyHighByte=(unsigned char *)(&PWM7DCH);	//store address of PWM duty high byte
    motorR.dir_LAT=(unsigned char *)(&LATG); 		//store address of LAT
    motorR.dir_pin=6; 						//pin RG6 controls direction
    motorR.PWMperiod=PWMcycle; 			//store PWMperiod for motor
    
    //definition of RGB structure
    struct RGB_val test;
    test.R = 0;
    test.G = 0;
    test.B = 0;
    
    // Turn on Front LEDs 
    LATGbits.LATG1=0;   //set initial output state
    TRISGbits.TRISG1=0; //set TRIS value for pin (output)
    LATAbits.LATA4=0;   //set initial output state
    TRISAbits.TRISA4=0; //set TRIS value for pin (output)
    LATFbits.LATF7=1;   //set initial output state
    TRISFbits.TRISF7=0; //set TRIS value for pin (output)
    
//    unsigned int count = 0;
//    unsigned int x = test.R;
//    unsigned int y = test.G;
//    unsigned int z = test.B;
    
    TRISDbits.TRISD7 = 0;
    LATDbits.LATD7 = 0;
    
    while(1){ 
        read_colours(&test); // For general light sensing
        LATDbits.LATD7 = 0;
        
        if (!PORTFbits.RF2){
            LATDbits.LATD7 = 1;
            LATGbits.LATG1=1;   // White Light
            LATAbits.LATA4=1;   
            LATFbits.LATF7=1; 
            __delay_ms(100);
            read_colours(&test);
            float temp = determine_color_new(&test);
            unsigned int int_part;
            unsigned int frac_part;
            int_part = temp/1;
            frac_part =(temp*1000)/1 - int_part*1000;
            sprintf(string," Hue1: %d.%03d",int_part, frac_part);
            TxBufferedString(string);
            sendTxBuf();
            __delay_ms(100);
            
            LATGbits.LATG1=1;   //Red Light
            LATAbits.LATA4=0;  
            LATFbits.LATF7=0;  
            __delay_ms(100);
            read_colours(&test);
            temp = determine_color_new(&test);
            int_part = temp/1;
            frac_part =(temp*1000)/1 - int_part*1000;
            sprintf(string," Hue2: %d.%03d",int_part, frac_part);
            TxBufferedString(string);
            sendTxBuf();
            __delay_ms(100);
            
            LATGbits.LATG1=0;   //Green Light
            LATAbits.LATA4=1;  
            LATFbits.LATF7=0;  
            __delay_ms(100);
            read_colours(&test);
            temp = determine_color_new(&test);
            int_part = temp/1;
            frac_part =(temp*1000)/1 - int_part*1000;
            sprintf(string," Hue3: %d.%03d",int_part, frac_part);
            TxBufferedString(string);
            sendTxBuf();
            __delay_ms(100);
            
            LATGbits.LATG1=0;   //set initial output state
            LATAbits.LATA4=0;   //set initial output state
            LATFbits.LATF7=0;   //set initial output state
        }
    }
}
