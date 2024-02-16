#include "msp.h"
#include "Pump.h"
#include "Time.h"
#include "Communication.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>


// Set Debounce values for script.
#define SYSTEM_CLOCK_FREQUENCY 3000  //kHz  (3MHz)
#define DEBOUNCE_PERIOD 1000 //milliseconds
#define DEBOUNCE_LOOP_CYCLES 8 //cycles
#define DEBOUNCE_LOOP_COUNT DEBOUNCE_PERIOD*SYSTEM_CLOCK_FREQUENCY/DEBOUNCE_LOOP_CYCLES
void debounce(void){
    int i;
    for (i=DEBOUNCE_LOOP_COUNT;i!=0; i--); // each incrament is 8 cycles
//deley time for debouncing switches
} //end debounce()

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	TimeLength testTime = {
	                       .day = 1,
	                       .hr = 0,
	                       .min = 2,
	                       .sec = 5,
	                       .ms = 12
	};

	char buffer[30];
	extern char* CommandBuffer;

	initCommunication();

	printf("hello world!\r\n");
	sprintf(CommandBuffer);
	printf("\r\n");

	addCharToCommandBuffer('a');
	sprintf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('b');
    sprintf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('c');
    sprintf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('d');
    sprintf(CommandBuffer);
    printf("\r\n");

    evaluateCommandBuffer();
    sprintf(CommandBuffer);
    printf("\r\n");

    addCharToCommandBuffer('s');
    sprintf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('t');
    sprintf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('p');
    sprintf(CommandBuffer);
    printf("\r\n");

    evaluateCommandBuffer();
    sprintf(CommandBuffer);
    printf("\r\n");

	//initAllPumps();

	while(1);
}

