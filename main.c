//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "clock.h"

void DisableWatchdog(void) {
	WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
}

void SelectPortFunction(int port,int line,int sel0,int sel1)
{
	if(port==1)
	{
		if(P1SEL0 & BIT(line)!=sel0)
		{
			if(P1SEL1 & BIT(line)!=sel1)
				P1SELC|=BIT(line);
			else
				P1SEL0^=BIT(line);
		}
		else
		{
			if(P1SEL1 & BIT(line)!=sel1)
				P1SEL1^=BIT(line);
		}
	}
	else if(port==2)
	{
		if(P2SEL0 & BIT(line)!=sel0)
		{
			if(P2SEL1 & BIT(line)!=sel1)
				P2SELC|=BIT(line);
			else
				P2SEL0^=BIT(line);
		}
		else
		{
			if(P2SEL1 & BIT(line)!=sel1)
				P2SEL1^=BIT(line);
		}
	}
	else if(port==3)
	{
		if (P3SEL0 & BIT(line) != sel0) {
			if (P3SEL1 & BIT(line) != sel1)
				P3SELC |= BIT(line);
			else
				P3SEL0 ^= BIT(line);
		} else {
			if (P3SEL1 & BIT(line) != sel1)
				P3SEL1 ^= BIT(line);
		}
	}
	else if(port==4)
	{
		if (P4SEL0 & BIT(line) != sel0) {
			if (P4SEL1 & BIT(line) != sel1)
				P4SELC |= BIT(line);
			else
				P4SEL0 ^= BIT(line);
		} else {
			if (P4SEL1 & BIT(line) != sel1)
				P4SEL1 ^= BIT(line);
		}
	}
	else if(port==5)
	{
		if (P5SEL0 & BIT(line) != sel0) {
			if (P5SEL1 & BIT(line) != sel1)
				P5SELC |= BIT(line);
			else
				P5SEL0 ^= BIT(line);
		} else {
			if (P5SEL1 & BIT(line) != sel1)
				P5SEL1 ^= BIT(line);
		}
	}
	else if(port==6)
	{
		if (P6SEL0 & BIT(line) != sel0) {
			if (P6SEL1 & BIT(line) != sel1)
				P6SELC |= BIT(line);
			else
				P6SEL0 ^= BIT(line);
		} else {
			if (P6SEL1 & BIT(line) != sel1)
				P6SEL1 ^= BIT(line);
		}
	}
}


void ConfigureIOPort(void) {
	SelectPortFunction(2,4,0,0);
	SelectPortFunction(2,6,0,0);
	SelectPortFunction(2,7,0,0);
	P2DIR|=(BIT4|BIT6|BIT7);

}

void ConfigureTimerA0(void) {
	TA0CTL = 0x0200;
	TA0CCTL0 = 0x2000;
	TA0CCR0 = 0x2710;//set frequency - will latch the data through

	TA0CCTL1=0x2010;
	TA0CCTL2=0x2010;
	TA0CCTL3=0x2010;
	TA0CCTL4=0x2010;
	
	TA0CCR1=0x00A0;
	TA0CCR2=0x0140;
	TA0CCR3=0x01E0;
	TA0CCR4=0x0280;
	
	TA0CTL = 0x0216;
}

unsigned char data[20]={7,9,11,13,14,14,14,13,11,9,7,5,3,1,0,0,0,1,3,5};
unsigned char currentByte = 0;
unsigned char currentBit = 3;

void writeNextBit(void){
	if(data[currentByte]&(1<<currentBit)){
		P2OUT&=~BIT4;
	}else{
		P2OUT|=BIT4;
	}//sets value on data line
	P2OUT&=~BIT6; //pulse clock
	P2OUT |= BIT6;
	if (currentBit == 0) {
		currentBit = 3;
		if (++currentByte == 20) {
			currentByte = 0;
		}
	} else {
		currentBit -= 1;
	}
}
void stepLatch(void) {
	P2OUT &=~BIT7;
	P2OUT|=BIT7;
}

void TimerA0Interrupt(void) {
	static int writeEnable=1;
	unsigned short val = TA0IV;
	switch(val){
	case 0x02:
	case 0x04:
	case 0x06:
	case 0x08:
		if(writeEnable){
			writeNextBit();
			if(val==0x08)
				writeEnable=0;
		}
		break;
	case 0x0E:
		stepLatch();
		writeEnable=1;
		break;
	}
}

void main(void)
{
	DisableWatchdog();
	SetClockFrequency();
	ConfigureIOPort();
	ConfigureTimerA0();
	NVIC_EnableIRQ(TA0_N_IRQn);
	while(1);
}
