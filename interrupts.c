#include <xc.h>
#include "interrupts.h"
#include "serial.h"
#include "dc_motor.h"
#include "color.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    PIE2bits.C1IE = 1; //to make use of comparator interrupt (interrupt source)
    PIE0bits.TMR0IE = 1; //make use of timer interrupt 
    INTCONbits.GIEL = 1; //turn on global peripheral interrupts
    INTCONbits.GIE = 1; //turn on interrupts globally (when this is off, all interrupts are deactivated))
    
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR() // If overrun by 20s, will then trigger memory 
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if(PIR0bits.TMR0IF){
        //LATHbits.LATH3 = !LATHbits.LATH3; //change the state of the LED (for testing))
        TMR0H = 0;
        TMR0L = 0;
        //White(&motorL, &motorR); 
        PIR0bits.TMR0IF = 0; // clearing the flag
    } 
}