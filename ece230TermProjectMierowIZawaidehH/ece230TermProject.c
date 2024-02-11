#include "msp.h"
#include "Pump.h"
#include "Time.h"

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//initAllPumps();

	while(1);
}
