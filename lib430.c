/*
 * lib430.c
 *
 *  Created on: Feb 6, 2016
 *      Author: Jason
 */


void (*TA0_timedFunctPtr)();
__interrupt void TA0_intFunct();

void TA0_init(){
	TA0_assignSourceClk(eSMCLK);
	TA0_assignPsk(ePSK1);
	TA0_setPeriod(10000);
	TA0_stop();

	TA0_timedFunctPtr = _null_ptr_;
}

void TA0_assignSourceClk(Clk clock){
	TA0CTL &= ~(TASSEL0 | TASSEL1);

	switch(clock){
		case eACLK:
		{
			TA0CTL |= TASSEL0;
			break;
		}
		case eSMCLK:
		{
			TA0CTL |= TASSEL1;
			break;
		}
		case eMCLK:
		{
			TA0CTL |= (TASSEL0 | TASSEL1);
			break;
		}
	}
}

void TA0_assignPsk(PreScaler psk){
	TA0CTL &= ~(ID0 | ID1);
	switch(psk){
		case ePSK8:
		{
			TA0CTL |= (ID0 | ID1);
			break;
		}
		case ePSK4:
		{
			TA0CTL |= ID1;
			break;
		}
		case ePSK2:
		{
			TA0CTL |= ID0;
			break;
		}
	}
}

PreScaler TA0_getPsk(){
	uint16_t pskReg = (TA0CTL & (ID0 | ID1));
	PreScaler psk;

	if(pskReg == (ID0 | ID1)){
		psk = ePSK8;
	}else if(pskReg == ID1){
		psk = ePSK4;
	}else if(pskReg == ID0){
		psk = ePSK2;
	}else{
		psk = ePSK1;
	}

	return psk;
}

uint8_t TA0_setPeriod(uint32_t microSeconds){
	uint8_t timerIsSet = 0x01;

	if(microSeconds < 65536){
		TA0_assignPsk(ePSK1);
	}else if(microSeconds < 131072){
		microSeconds = microSeconds >> 1;
		TA0_assignPsk(ePSK2);
	}else if(microSeconds < 262144){
		microSeconds = microSeconds >> 2;
		TA0_assignPsk(ePSK4);
	}else if(microSeconds < 524288){
		microSeconds = microSeconds >> 3;
		TA0_assignPsk(ePSK8);
	}else{
		microSeconds = 65535;
		TA0_assignPsk(ePSK8);
		timerIsSet = 0x00;
	}

	TA0CCR0 = (uint16_t)microSeconds;

	return timerIsSet;
}

uint32_t TA0_getPeriod(){
	uint32_t microSeconds;
	if(TA0_getPsk() == ePSK1){
		microSeconds = (uint32_t)TA0CCR0;
	}else if(TA0_getPsk() == ePSK2){
		microSeconds = (uint32_t)TA0CCR0 << 1;
	}else if(TA0_getPsk() == ePSK4){
		microSeconds = (uint32_t)TA0CCR0 << 2;
	}else if(TA0_getPsk() == ePSK8){
		microSeconds = (uint32_t)TA0CCR0 << 3;
	}else{
		microSeconds = 0xffffffff;
	}

	return microSeconds;
}

void TA0_enableInterrupt(void (*functPtr)()){
	TA0_timedFunctPtr = functPtr;
	TA0CCTL0 |= CCIE;
	__bis_SR_register(GIE);
}

void TA0_disableInterrupt(){
	TA0CCTL0 &= ~CCIE;
}

void TA0_run(){
	TA0CTL |= MC0;
	TA0CTL &= ~MC1;
}

void TA0_reset(){
	TA0CTL |= TACLR;
}

void TA0_stop(){
	TA0CTL &= ~(MC0 | MC1);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_intFunct(){
	if(TA0_timedFunctPtr != 0)
		(*TA0_timedFunctPtr)();

	TA0IV = 0;
}


