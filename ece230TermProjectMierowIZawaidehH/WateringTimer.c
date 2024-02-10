/*
 * WateringTimer.c
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */

#include "Time.h"
#include "Pump.h"
#include "msp.h"


/*  Timer Configuration Variables:
 *
 *  The number of the timer is the number of the watering system that is should be watering
 *
 *  TimerXWaitLength:
 *      This is the amount of time the timer will wait between ordering the Watering System to water again.
 *  TimerXWateringLength:
 *      This is the length of time the timer will run between telling the Watering System to Start Watering and Stop Watering.
 */
TimeLength Timer1WaitLength;
TimeLength Timer1WateringLength;

void convertTimerLengthToTicks(TimeLength *time, TimerSettings *settingToChange);

/* Timer Tick Objects:
 *
 * These objects are the objects that store the tick information used to operate the timers.
 * These are interpreted from the TimeLength values stored above.
 *
 */
typedef struct{
    unsigned int fullRunCount; //how many full runs we need
    unsigned short additionalTicks; // number of extra ticks for final run (this will not be a complete run)
}TimerSettings;

typedef struct{
    unsigned volatile int fullRunsRemaining; //variable that counts down from fullRunCount and holds number of full runs left
    unsigned volatile short finalRunTicks; // stores the number that we are comparing against in our final run to know when to interrupt the timer
}TimerValues;

typedef struct{
    uint16_t *CCR; //CCR register - allows us to update CCRy value for specific timer
    uint16_t *CCTL; //CCTL timer of specific timer
    uint16_t InterruptMask; //specify bit that sets timer interrupt (specific timer) - bit interrupt is in to reset it [set compare capture interrupt flag]
    uint16_t InterruptEnableMask; // CCIE
}TimerRegister;


typedef struct { //The KING
    TimerRegister Reg;
    TimerSettings WateringSettings;
    TimerSettings WaitingSettings;
    TimerValues   ActiveValues;
    PumpInfo    *Pump;
}TimerData;



void initWateringTimer(void);

//concludes a full timer run cycle (0xFFFF), decrements currentFullRunCount and begins final run or switches state if needed
void completeFullRunTasks_interrupt(TimerData *timer);



//concludes the final run (partial cycle),
void completePartialRunTasks_interrupt(TimerData *timer);


//initiate final run timer, enables CCR, CCIFG, (CCIE flag needed?)
/*
 * Will initiate final run and set ccr value to appropriate value
 */
void initFinalRun_interrupt(TimerData *timer);


/*
 * Toggle pump, switch active state of timer and pump (wait<-->water), switch timer mode, recalculate  active values for next run
 */
void startTimerCycle_interrupt(TimerData *timer);

/*
 * using the current timer location, use the proper offset for desired time length, be able to refer to correct register to get current number of ticks
 */
void recalculateActiveValues(TimerData *timer);

#define WTimerCounterRegister TIMER_A3->R

void TA3_0_IRQHandler(void);
void TA3_N_IRQHandler(void);
__enable_irq(); //enable global interrupt

