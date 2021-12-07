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

    // setup pin for input (connected to left button)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin
    
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
    test.C = 0;
    test.R = 0;
    test.G = 0;
    test.B = 0;
    
    // Turn on Front LEDs 
    LATGbits.LATG1=0;   //set initial output state
    TRISGbits.TRISG1=0; //set TRIS value for pin (output)
    LATAbits.LATA4=0;   //set initial output state
    TRISAbits.TRISA4=0; //set TRIS value for pin (output)
    LATFbits.LATF7=0;   //set initial output state
    TRISFbits.TRISF7=0; //set TRIS value for pin (output)
    
    // LED on board
    TRISDbits.TRISD7 = 0;
    LATDbits.LATD7 = 0;
    
    // LED on board
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
    
    char string[20];
    char string0[20];
    char string1[20];
    char string2[20];
    char string3[20];
    float RedRatio, GreenRatio, BlueRatio;
   
    unsigned int cal = 0;
    
    // Calibration done against White Card
    while(cal==0){
        LATDbits.LATD7 = 1;
        while (PORTFbits.RF2); //empty while loop (wait for button press)
        if (!PORTFbits.RF2){
            LATDbits.LATD7 = 0;
            LATGbits.LATG1=1;   // White Light
            LATAbits.LATA4=1;   
            LATFbits.LATF7=0; 
            calibrate(&test);
            __delay_ms(300);
            LATGbits.LATG1=0;   // White Light
            LATAbits.LATA4=0;   
            LATFbits.LATF7=0; 
             __delay_ms(300);
            }   
        
        sprintf(string0," AmbR:%d G:%d B:%d \r\n",test.ambientR,test.ambientG,test.ambientB);
        TxBufferedString(string0);
        sendTxBuf();
        __delay_ms(50);
        
        LATHbits.LATH3 = 1;
        while (PORTFbits.RF3);
        if (!PORTFbits.RF3){
            LATHbits.LATH3 = 0;
            cal = 1;
        }
    }
    
    while(1){          
        unsigned int output;
        LATGbits.LATG1=1;   // Using White Light
        LATAbits.LATA4=1;   
        LATFbits.LATF7=0; 
        read_colours(&test);
        output = determine_color_new(&test); 
        RedRatio = ((float)test.R) / ((float)test.C);
        GreenRatio = ((float)test.G) / ((float)test.C);
        BlueRatio = ((float)test.B) / ((float)test.C);
        __delay_ms(50);
        LATGbits.LATG1=0;   //set initial output state
        LATAbits.LATA4=0;   //set initial output state
        LATFbits.LATF7=0;   //set initial output state
        __delay_ms(50);

        unsigned int int_part1 = RedRatio/1;
        unsigned int frac_part1 =(RedRatio*1000)/1 - int_part1*1000;
        sprintf(string1," R:%d.%03d ",int_part1, frac_part1);
        TxBufferedString(string1);
        sendTxBuf();
        __delay_ms(50);
        
        unsigned int int_part2 = GreenRatio/1;
        unsigned int frac_part2 =(GreenRatio*1000)/1 - int_part2*1000;
        sprintf(string2," G:%d.%03d ",int_part2, frac_part2);
        TxBufferedString(string2);
        sendTxBuf();
        __delay_ms(50);
        
        unsigned int int_part3 = BlueRatio/1;
        unsigned int frac_part3 =(BlueRatio*1000)/1 - int_part3*1000;
        sprintf(string3," B:%d.%03d ",int_part3, frac_part3);
        TxBufferedString(string3);
        sendTxBuf();
        __delay_ms(50);
        
        sprintf(string," Color:%d \r\n",output);
        TxBufferedString(string);
        sendTxBuf();
        __delay_ms(50);
    }
}
