#include <xc.h>
#include "interrupts.h"
#include "serial.h"

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
//void __interrupt(high_priority) HighISR()
//{
//	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
//    if(PIR4bits.RC4IF == 1){
//        putCharToRxBuf(RC4REG);
//        //PIR4bits.RC4IF = 0; // clearing the flag, but don't need this because the above line will clear the flag automatically
//    }
//    if(PIR4bits.TX4IF == 1){ //TX4RED is empty
//        if (PIR4bits.TX4IF == 1 && PIE4bits.TX4IE== 1){
//            TX4REG = getCharFromTxBuf();
//        }
//        else{
//        PIE4bits.TX4IE=0;
//        }
//    }   
//}