/*
 * WateringTimer.c
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */


#include <msp.h>
#include "Pump.h"
#include "WateringTimer.h"
#include "Time.h"

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

void convertTimerLengthToTicks(TimeLength *time, TimerSettings *settingToChange);
void convertTimerLengthToTicks(TimeLength *time, TimerSettings *settingToChange){
    long long totalTicks=0;
    int ticks_s = TimerA3Clock; //ticks per second
    int ticks_ms = 0.001*ticks_s;
    int ticks_m =60*ticks_s;
    int ticks_h = 60*ticks_h;
    int ticks_d = 24*ticks_h;
    totalTicks = (*time->ms *(ticks_ms))+
            (*time->sec *(ticks_s))+
            (*time->hr *(ticks_h)) +
            (*time->min)*(ticks_m)+
            (*time->day *(ticks_d));
    additionalTicks = totalTicks%(2^16);
    fullRunCount = (totalTicks-additionalTicks)/(2^16);

}


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
    extern PumpInfo *Pump;
}TimerData;



void initWateringTimer(void);
void initWateringTimer(){
    //TODO fix cstl1 line to set smclk correctly
    CSCTL1->SELS=0b001; //set SMCLK source to VLOCLK

    /*
     * Bits 9-8 (0b10) smclk, 7-6(0b11) div 8, 5-4 (0b10)cont mode,
     * 3- reserved, 2(0b1) clear,
     * 1 (0b1) TAIFG interrupt enables, 0(0b0) TAIF no interrupt pending
     */
    TIMER_A3->CTL|=0x2E6; //0b1011100110
    TIMER_A3->EX0|=0b111; //divide by 8

    TIMER_A3->CCTL[0] = 0b0;
    TIMER_A3->CCTL[1] = 0b0;
    TIMER_A3->CCTL[2] = 0b0;
    TIMER_A3->CCTL[3] = 0b0;
    TIMER_A3->CCTL[4] = 0b0;

}


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
void startTimerCycle_interrupt(TimerData *timer){
    togglePump(*timer->Pump); //toggle pump status
    if(*timer->Pump->IsActive){
        recalculateActiveValues(*timer->WateringSettings, *timer->ActiveValues);

    }else{
        recalculateActiveValues(*timer->WaitingSettings, *timer->ActiveValues);
    }

}

/*
 * using the current timer location, use the proper offset for desired time length, be able to refer to correct register to get current number of ticks
 */
void recalculateActiveValues(TimerSettings *length, TimerValues *valuesToChange);
void recalculateActiveValues(TimerSettings *length, TimerValues *valuesToChange){
    //int currentTicks = TIMER_A3->R;
    int currentTicks = *WTimerCounterRegister;
    if(0xFFFF-currentTicks< (*length->additionalTicks)){
        *valuesToChange->fullRunsRemaining=*length->fullRunCount+1;
        *valuesToChange->finalRunTicks = *length->additionalTicks -(0xFFFF-currentTicks);
    }else{
        *valuesToChange->fullRunsRemaining=*length->fullRunCount;
        *valuesToChange->finalRunTicks = *length->additionalTicks + currentTicks;
    }
}


#define WTimerCounterRegister TIMER_A3->R

void TA3_0_IRQHandler(void);
void TA3_N_IRQHandler(void);
//__enable_irq(); //enable global interrupt

