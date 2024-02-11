#include "msp.h"
#include "Pump.h"

extern PumpInfo Pump1;
extern PumpInfo Pump2;
extern PumpInfo Pump3;


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	initAllPumps();

	turnOnPump(&Pump1);
	togglePump(&Pump2);
	while(1);
}
