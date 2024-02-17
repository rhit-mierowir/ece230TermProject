#include "msp.h"
#include "Pump.h"
#include "Time.h"
#include "Communication.h"
#include "WateringTimer.h"
#include "LevelSensor.h"
#include "Switches.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>


//// Set Debounce values for script.
//#define SYSTEM_CLOCK_FREQUENCY 3000  //kHz  (3MHz)
//#define DEBOUNCE_PERIOD 1000 //milliseconds
//#define DEBOUNCE_LOOP_CYCLES 8 //cycles
//#define DEBOUNCE_LOOP_COUNT DEBOUNCE_PERIOD*SYSTEM_CLOCK_FREQUENCY/DEBOUNCE_LOOP_CYCLES
//void debounce(void){
//    int i;
//    for (i=DEBOUNCE_LOOP_COUNT;i!=0; i--); // each incrament is 8 cycles
////deley time for debouncing switches
//} //end debounce()

extern TimerData Timer1;
extern PumpInfo Pump0, Pump1, Pump2, Pump3, Pump4;



/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    extern char CommandBuffer[];
	initCommunication();

	printf("hello world!\r\n");
	printf(CommandBuffer);
	printf("\r\n");

	addCharToCommandBuffer('a');
	printf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('b');
    printf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('3');
    printf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('l');
    printf(CommandBuffer);
    printf("\r\n");

    evaluateCommandBuffer();
    printf(CommandBuffer);
    printf("\r\n");

    addCharToCommandBuffer('s');
    printf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('t');
    printf(CommandBuffer);
    printf("\r\n");
    addCharToCommandBuffer('p');
    printf(CommandBuffer);
    printf("\r\n");

    evaluateCommandBuffer();
    printf(CommandBuffer);
    printf("\r\n");



    TimeLength waitTime = {
                           .day = 0,
                           .hr = 0,
                           .min = 0,
                           .sec = 10,
                           .ms = 0
    };
    TimeLength waterTime = {
                           .day = 0,
                           .hr = 0,
                           .min = 0,
                           .sec = 8,
                           .ms = 0
    };

    initWateringTimer();
    initAllPumps();
    configureLevelSensor();
    Timer1.TimerTimes.WaitLength=waitTime;
    Timer1.TimerTimes.WateringLength=waterTime;
    updateTimerTickSettings(&Timer1);

    //test
    //completePartialRunTasks_interrupt(&Timer1);



	char buffer[30];



	//initAllPumps();

	while(1){
	    levelSwitchTasks();
	    performAllPlantButtonsTasks();
	    masterSwitchTasks();


	}
}

