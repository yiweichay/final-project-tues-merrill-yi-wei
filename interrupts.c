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
    INTCONbits.PEIE=1; 	//enable peripheral interrupts
    PIE4bits.RC4IE=1;	//receive interrupt
    PIE4bits.TX4IE=0;	//transmit interrupt (only turn on when you have more than one byte to send)
    INTCONbits.GIE=1; 	//enable interrupts globally 
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{   //add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...    
    if (PIR4bits.RC4IF == 1){ 
        putCharToRxBuf(RC4REG);
    }
    if (PIR4bits.TX4IF  == 1 && PIE4bits.TX4IE==1){ // TX4REG is empty
        if (isDataInTxBuf()==1 ) {TX4REG = getCharFromTxBuf();} // Set next char 
        else {PIE4bits.TX4IE=0;}
    }
}

