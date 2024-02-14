/*
 * PlantTimer.h
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */

#ifndef WATERINGTIMER_H_
#define WATERINGTIMER_H_

#include "Pump.h"
#include "Time.h"

#define SMCLK 32000 //Hz <-- USe ACLK
#define TimerA3Prescaler 64 //8*8
#define TimerA3Clock SMCLK/TimerA3Prescaler //ticks per second
#define TimerA3Ticklength = TimerA3Clock^(-1) //seconds per tick

#define DELTA 2

#define interruptMask 0b0
#define interruptEnableMask 0b10

#define WTimer TIMER_A3
#define WTimerCounterRegister TIMER_A3->R

#define Timer0CCR WTimer->CCR[0]
#define Timer1CCR WTimer->CCR[1]
#define Timer2CCR WTimer->CCR[2]
#define Timer3CCR WTimer->CCR[3]
#define Timer4CCR WTimer->CCR[4]
#define Timer0CCTL WTimer->CCTL[0]
#define Timer1CCTL WTimer->CCTL[1]
#define Timer2CCTL WTimer->CCTL[2]
#define Timer3CCTL WTimer->CCTL[3]
#define Timer4CCTL WTimer->CCTL[4]


/*  Timer Configuration Variables:
 *
 *  The number of the timer is the number of the watering system that is should be watering
 *
 *  TimerXWaitLength:
 *      This is the amount of time the timer will wait between ordering the Watering System to water again.
 *  TimerXWateringLength:
 *      This is the length of time the timer will run between telling the Watering System to Start Watering and Stop Watering.
 */
typedef struct{
    TimeLength WaitLength;
    TimeLength WateringLength;
}TimerDuration;

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
    volatile uint16_t *CCR; //CCR register - allows us to update CCRy value for specific timer
    volatile uint16_t *CCTL; //CCTL timer of specific timer
    uint16_t InterruptMask; //specify bit that sets timer interrupt (specific timer) - bit interrupt is in to reset it [set and reset compare capture interrupt flag]
    uint16_t InterruptEnableMask; // CCIE -  enable and disable interrupt
}TimerRegister;




/* KING STRUCT
                  .       |         .    .
            .  *         -*-          *
                 \        |         /   .
.    .            .      /^\     .              .    .
   *    |\   /\    /\  / / \ \  /\    /\   /|    *
 .   .  |  \ \/ /\ \ / /     \ \ / /\ \/ /  | .     .
         \ | _ _\/_ _ \_\_ _ /_/_ _\/_ _ \_/
           \  *  *  *   \ \/ /  *  *  *  /
            ` ~ ~ ~ ~ ~  ~\/~ ~ ~ ~ ~ ~ '
 */
typedef struct { //The KING
    TimerRegister Reg;
    TimerDuration TimerTimes;
    TimerSettings WateringSettings;
    TimerSettings WaitingSettings;
    TimerValues   ActiveValues;
    PumpInfo *Pump;
}TimerData;



void convertTimerLengthToTicks(TimeLength *time, TimerSettings *settingToChange);
void updateTimerTickSettings(TimerData *timer);
void initWateringTimer(void);
void recalculateActiveValues(TimerSettings *length, TimerValues *valuesToChange);
void startTimerCycle_interrupt(TimerData *timer);
void initFinalRun_interrupt(TimerData *timer);
void completePartialRunTasks_interrupt(TimerData *timer);
void completeFullRunTasks_interrupt(TimerData *timer);
void TA3_0_IRQHandler(void);
void TA3_N_IRQHandler(void);






//code goes above endif statement
#endif /* WATERINGTIMER_H_ */
