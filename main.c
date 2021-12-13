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

unsigned int movements = 0;
unsigned int timerArray[10] = {};
unsigned int movementArray[10] = {};

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    // Initialise Helper Scripts
    initDCmotorsPWM(99);
    color_click_init();
    initUSART4(); 
    Timer0_init();
    Interrupts_init();
    
    // Motor Structs Initialisation 
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
     
    // Initialisation of RGB structure
    struct RGB_val RGBstruct;
    RGBstruct.C = 0;
    RGBstruct.R = 0;
    RGBstruct.G = 0;
    RGBstruct.B = 0;
    
     // setup buttons for input
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin
    
    // Initialise Front LEDs on ColourClick 
    LATGbits.LATG1=1;   //set initial output state
    TRISGbits.TRISG1=0; //set TRIS value for pin (output)
    LATAbits.LATA4=1;   //set initial output state
    TRISAbits.TRISA4=0; //set TRIS value for pin (output)
    LATFbits.LATF7=1;   //set initial output state
    TRISFbits.TRISF7=0; //set TRIS value for pin (output)
    
    // Indicator LEDs on board
    TRISDbits.TRISD7 = 0; LATDbits.LATD7 = 0;
    TRISHbits.TRISH3 = 0; LATHbits.LATH3 = 0;
    
    // Calibration Function Each Start Up
    unsigned int cal = 0;
    while(cal==0){
        LATDbits.LATD7 = 1;
        while (PORTFbits.RF2); // Press to Calibrate White
        if (!PORTFbits.RF2){
            LATDbits.LATD7 = 0; // LED flicker to show process done
            calibrateW(&RGBstruct);
            __delay_ms(300);
            }   
        
        LATDbits.LATD7 = 1;
        while (PORTFbits.RF2); // Press to Calibrate Black
        if (!PORTFbits.RF2){
            LATDbits.LATD7 = 0; 
            calibrateB(&RGBstruct);
            __delay_ms(300);
            }   
        
        LATHbits.LATH3 = 1;
        while (PORTFbits.RF3); // Press to start maze
        if (!PORTFbits.RF3){
            LATHbits.LATH3 = 0;
            TMR0H = 0;
            TMR0L = 0;
            cal = 1;
        }
    }
    
    // Initialise Variables for use in Main
    unsigned int check1 = 9; unsigned int check2 = 9; // Used for colour checks
    unsigned int check3 = 9; unsigned int check4 = 9; // Used for colour checks
    unsigned int count = 0;             // Used for colour checks
    unsigned int reset_timer = 1;       
    
    while(1){          
        // Scans once and updates a checkX accordingly
        unsigned int detected_colour;  
        read_colours(&RGBstruct);        // Updates RGB Struct
        if (count==0) {check1 = determine_color_new(&RGBstruct);}
        if (count==1) {check2 = determine_color_new(&RGBstruct);}
        if (count==2) {check3 = determine_color_new(&RGBstruct);}
        if (count==3) {check4 = determine_color_new(&RGBstruct);count=0;}
        else (count += 1);
        
        // 4 checks must be identical to detect colour (ROBUST)
        if (check1==check2 && check2==check3 && check3==check4){
            detected_colour = check1;
            if (detected_colour >= 0 && detected_colour <= 8){ 
                updateMovementCount(detected_colour, movementArray, movements, timerArray);
                movements++;
                reset_timer = 1; //reset the timer when the movement is done
            }
            else if (detected_colour == 9 && reset_timer == 1){ //if first detected ambient light after card read, reset timer
                
                TMR0H = 0;
                TMR0L = 0;
                reset_timer = 0; 
            }
            check1=9;check2=9;check3=9;check4=9; // Resets checks to ensure action done once 
        }
        
        if (detected_colour == 0){ turnRight90(&motorL,&motorR);__delay_ms(100);}           // Red - right 90
        if (detected_colour == 1){ turnLeft90(&motorL,&motorR);__delay_ms(100);}            // Blue - left 90
        if (detected_colour == 2){ turnRight180(&motorL,&motorR);__delay_ms(100);}          // Green - 180
        if (detected_colour == 3){ reverseTurnRight90(&motorL,&motorR);__delay_ms(100);}    // Yellow - reverse + right 90
        if (detected_colour == 4){ reverseTurnLeft90(&motorL,&motorR);__delay_ms(100);}     // Pink - reverse + left 90
        if (detected_colour == 5){ turnRight135(&motorL,&motorR);__delay_ms(100);}          // Orange - right 135
        if (detected_colour == 6){ turnLeft135(&motorL,&motorR);__delay_ms(100);}           // Light Blue - left 135
        
        // If black or white, head back to starting point 
        if (detected_colour == 7 || detected_colour == 8){ White(&motorL,&motorR,movementArray, movements, timerArray);
            __delay_ms(100);LATDbits.LATD7 = 1;LATHbits.LATH3 = 1; // Display to show done with course
            while (PORTFbits.RF3);}
        if (detected_colour == 9){ forward(&motorL,&motorR);}                               // Ambient - continue forward
    }
}
