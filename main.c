#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
#include "task.h"

/* defines to make things a bit more readable */
#define LED1_ON		(P1OUT |= BIT0)
#define LED1_OFF	(P1OUT &= ~BIT0)
#define LED1_IS_ON	((P1OUT & BIT0) > 0)

#define LED2_ON		(P1OUT |= BIT6)
#define LED2_OFF	(P1OUT &= ~BIT6)
#define LED2_IS_ON	((P1OUT & BIT6) > 0)

#define LED_1_BLINK_PERIOD	128
#define LED_2_BLINK_PERIOD	256
#define BTN_POLL_PERIOD		1

#define SWITCH_IS_ON	((P1IN & BIT3) > 0)

/* function declarations - each will be assigned as a 'task' */
void blinkLed1(void);
void blinkLed2(void);
void debounceButton(void);

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    TASK_init();

    /* setup the LED pins and switch */
    P1DIR &= ~BIT3;
	P1DIR |= (BIT0 | BIT6);
	LED1_OFF;
	LED2_OFF;

	/* add the debounce task */
	TASK_add(&debounceButton, BTN_POLL_PERIOD);

	/* enter the task execute - this will never return! */
    TASK_manage();

	return 0;
}

void blinkLed1(void){
	if(LED1_IS_ON){
		LED1_OFF;
	}else{
		LED1_ON;
	}
}

void blinkLed2(void){
	if(LED2_IS_ON){
		LED2_OFF;
	}else{
		LED2_ON;
	}
}

void debounceButton(void){
	static bool btnState = false;
	static int8_t btnCounter = 0;
	const int8_t btnCounterMax = 16;
	static bool ledState = false;

	/* when switch is on, increment btnCounter, else decrement */
	if(SWITCH_IS_ON){
		btnCounter++;
	}else{
		btnCounter--;
	}

	if((btnCounter == btnCounterMax) && (btnState == false)){
		/* the btnCounter just counted up to this point and this
		 * is the positive going edge - do something here */
		btnState = true;
		if(ledState){
			TASK_remove(&blinkLed2);
			TASK_add(&blinkLed1, LED_1_BLINK_PERIOD);
			LED2_OFF;
			ledState = false;
		}else{
			TASK_remove(&blinkLed1);
			TASK_add(&blinkLed2, LED_2_BLINK_PERIOD);
			LED1_OFF;
			ledState = true;
		}
	}else if(btnCounter > btnCounterMax){
		btnState = true;
		btnCounter = btnCounterMax;
	}else if(btnCounter < -btnCounterMax){
		btnState = false;
		btnCounter = -btnCounterMax;
	}
}
