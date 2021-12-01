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
        read_colours(&test);
        LATDbits.LATD7 = 0;
        if (!PORTFbits.RF2){
            unsigned int testresults[3] = {0,0,0};
            unsigned int test1[3] = {0,0,0};
            unsigned int test2[3] = {0,0,0};
            unsigned int test3[3] = {0,0,0};
            
            LATDbits.LATD7 = 1;
            LATGbits.LATG1=1;   //set initial output state
            LATAbits.LATA4=1;   //set initial output state
            LATFbits.LATF7=1;   //set initial output state
            read_colours(&test);
            test1[0] = test.R;
            test1[1] = test.G;
            test1[2] = test.B;
            testresults[0] = determine_color1(&test);
            __delay_ms(100);
            LATGbits.LATG1=1;   //set initial output state
            LATAbits.LATA4=0;   //set initial output state
            LATFbits.LATF7=0;   //set initial output state
            read_colours(&test);
            test2[0] = test.R;
            test2[1] = test.G;
            test2[2] = test.B;
            testresults[1] = determine_color2(&test);
            __delay_ms(100);
            LATGbits.LATG1=0;   //set initial output state
            LATAbits.LATA4=1;   //set initial output state
            LATFbits.LATF7=0;   //set initial output state
            read_colours(&test);
            test3[0] = test.R;
            test3[1] = test.G;
            test3[2] = test.B;
            testresults[2] = determine_color3(&test);
            __delay_ms(100);
            LATGbits.LATG1=0;   //set initial output state
            LATAbits.LATA4=0;   //set initial output state
            LATFbits.LATF7=0;   //set initial output state
            unsigned int out;

            int count = sizeof(testresults) / sizeof(testresults[0]);
            for (int i = 0; i < count - 1; i++) { // read comment by @nbro
                for (int j = i + 1; j < count; j++) {
                    if (testresults[i] == testresults[j]) {
                        out = testresults[i]; // 2 tests same 
                    }
                }
            }
            
            unsigned int x = testresults[0];
            unsigned int y = testresults[1];
            unsigned int z = testresults[2];
            sprintf(string," T1:%d T2:%d T3:%d ",x,y,z);
            TxBufferedString(string);
            sendTxBuf();
            __delay_ms(100);
            
            unsigned int a = test1[0];
            unsigned int b = test1[1];
            unsigned int c = test1[2];
            sprintf(string1," T1r:%d T1g:%d T1b:%d ",a,b,c);
            TxBufferedString(string1);
            sendTxBuf();
            __delay_ms(100);
            
            unsigned int d = test2[0];
            unsigned int e = test2[1];
            unsigned int f = test2[2];
            sprintf(string2," T2r:%d T2g:%d T2b:%d ",d,e,f);
            TxBufferedString(string2);
            sendTxBuf();
            __delay_ms(100);
            
            unsigned int g = test3[0];
            unsigned int h = test3[1];
            unsigned int i = test3[2];
            sprintf(string3," T3r:%d T3g:%d T3b:%d ",g,h,i);
            TxBufferedString(string3);
            sendTxBuf();
            __delay_ms(100);
            
//            sprintf(string," Colour:%d ",out);
            
        }
    }
}
