/*
 * lib430.c
 *
 *  Created on: Feb 6, 2016
 *      Author: Jason
 */

#include <msp430.h>


void (*TMR_timedFunctPtr)();
__interrupt void TMR_intFunct();

void TMR_init(void (*functPtr)()){
	TMR_timedFunctPtr = functPtr;

	TA0CTL |= TASSEL1;	/* SMCLK */
	TACCR0 = 2048;	/* timer period */

	TA0CTL |= MC0;
	TA0CTL &= ~MC1;

	TA0CCTL0 |= CCIE;
	__bis_SR_register(GIE);
}

void TMR_disableInterrupt(){
	TA0CCTL0 &= ~CCIE;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TMR_intFunct(){
	if(TMR_timedFunctPtr != 0)
		(*TMR_timedFunctPtr)();

	TA0IV = 0;
}


