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
#include "csLFXT.h"

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


//Declare structs for all timers
TimerData Timer0 = {
                    .Reg = {
                        .CCR = &(Timer0CCR),
                        .CCTL = &(Timer0CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
                    //let timer times be undeclared.
                   // .Pump = *allPumps[0]
};
TimerData Timer1 = {
                    .Reg = {
                        .CCR = &(Timer1CCR),
                        .CCTL = &(Timer1CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
};
TimerData Timer2 = {
                    .Reg = {
                        .CCR = &(Timer2CCR),
                        .CCTL = &(Timer2CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
};
TimerData Timer3 = {
                    .Reg = {
                        .CCR = &(Timer3CCR),
                        .CCTL = &(Timer3CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
};
TimerData Timer4 = {
                    .Reg = {
                        .CCR = &(Timer4CCR),
                        .CCTL = &(Timer4CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
};


void convertTimerLengthToTicks(TimeLength *time, TimerSettings *settingToChange);
void convertTimerLengthToTicks(TimeLength *time, TimerSettings *settingToChange){
    long long totalTicks=0;
    int ticks_s = TimerA3Clock; //ticks per second
    int ticks_ms = 0.001*ticks_s;
    int ticks_m =60*ticks_s;
    int ticks_h = 60*ticks_m;
    int ticks_d = 24*ticks_h;
    totalTicks = (time->ms *(ticks_ms))+
            (time->sec *(ticks_s))+
            (time->hr *(ticks_h)) +
            (time->min)*(ticks_m)+
            (time->day *(ticks_d));
    settingToChange->additionalTicks = totalTicks%(2^16);
    settingToChange->additionalTicks = (totalTicks-settingToChange->additionalTicks)/(2^16);

}


/*
 * Updates the number of ticks for waiting and watering.
 * Should be called when the TimeLength is updated
 */
void updateTimerTickSettings(TimerData *timer);
void updateTimerTickSettings(TimerData *timer){
    convertTimerLengthToTicks(&(timer->TimerTimes.WateringLength),&(timer->WateringSettings));
    convertTimerLengthToTicks(&(timer->TimerTimes.WaitLength), &(timer->WaitingSettings));
}


void initWateringTimer(void);
void initWateringTimer(){
    //Configure LFXT to use 32kHz crystal, source to ACLK
    configLFXT();
    /*
     * Bits 9-8 (0b01) aclk, 7-6(0b11) div 8, 5-4 (0b10)cont mode,
     * 3- reserved, 2(0b1) clear,
     * 1 (0b1) TAIFG interrupt enables, 0(0b0) TAIF no interrupt pending
     */
    TIMER_A3->CTL|=0x1E6; //0b0111100110
    TIMER_A3->EX0|=0b111; //divide by 8

    TIMER_A3->CCTL[0] = 0b0;
    TIMER_A3->CCTL[1] = 0b0;
    TIMER_A3->CCTL[2] = 0b0;
    TIMER_A3->CCTL[3] = 0b0;
    TIMER_A3->CCTL[4] = 0b0;

}


/*
 * using the current timer location, use the proper offset for desired time length, be able to refer to correct register to get current number of ticks
 */
void recalculateActiveValues(TimerSettings *length, TimerValues *valuesToChange);
void recalculateActiveValues(TimerSettings *length, TimerValues *valuesToChange){
    //int currentTicks = TIMER_A3->R;
    int currentTicks = WTimerCounterRegister;
    //if not enough additional ticks left before completing full run
    if(0xFFFF-currentTicks< (length->additionalTicks)){
        valuesToChange->fullRunsRemaining= length->fullRunCount+1;
        valuesToChange->finalRunTicks = length->additionalTicks -(0xFFFF-currentTicks);
   //if can just add additional ticks to current tick count without overflow.
    }else{
        valuesToChange->fullRunsRemaining=length->fullRunCount;
        valuesToChange->finalRunTicks = length->additionalTicks + currentTicks;
    }
}

/*
 * Toggle pump, switch active state of timer and pump (wait<-->water), switch timer mode, recalculate  active values for next run
 */
void startTimerCycle_interrupt(TimerData *timer);
void startTimerCycle_interrupt(TimerData *timer){
    togglePump(timer->Pump); //toggle pump status
    //is active stores pump status for next run
    if(timer->Pump->IsActive){
        recalculateActiveValues((&timer->WateringSettings), &(timer->ActiveValues));

    }else{
        recalculateActiveValues(&(timer->WaitingSettings), &(timer->ActiveValues));
    }

}

//initiate final run timer, enables CCR, CCIFG, (CCIE flag needed?)
/*
 * Will initiate final run and set ccr value to appropriate value
 */

void initFinalRun_interrupt(TimerData *timer);
void initFinalRun_interrupt(TimerData *timer){
    //clear the interrupt flag.
    *(timer->Reg.CCTL) &= ~(timer->Reg.InterruptMask);
    //enable TAxCCTLy CCIFG for the particular pump
    *(timer->Reg.CCTL) |= timer->Reg.InterruptEnableMask;


    //TAxCCRy = finalRunTicks
    *(timer->Reg.CCR) =timer->ActiveValues.finalRunTicks;
}




//concludes the final run (partial cycle),
void completePartialRunTasks_interrupt(TimerData *timer);
void completePartialRunTasks_interrupt(TimerData *timer){
    *(timer->Reg.CCTL) &= ~(timer->Reg.InterruptMask);// clear interrupt flag
    startTimerCycle_interrupt(timer);
    //Check what to do based on newly generated timer settings.
    if(timer->ActiveValues.fullRunsRemaining==0){
        // no complete loops necessary
        initFinalRun_interrupt(timer);
    }else{
        // will perform full loops, disable individual interrupt to allow this
        *(timer->Reg.CCTL)&= ~(timer->Reg.InterruptEnableMask); //turn off enable
    }

}


//concludes a full timer run cycle (0xFFFF), decrements currentFullRunCount and begins final run or switches state if needed
void completeFullRunTasks_interrupt(TimerData *timer);
void completeFullRunTasks_interrupt(TimerData *timer){
    timer->ActiveValues.fullRunsRemaining--;
    if(timer->ActiveValues.fullRunsRemaining==0){
        if((timer->ActiveValues.finalRunTicks)<= DELTA){
            startTimerCycle_interrupt(timer);
        }else{
            initFinalRun_interrupt(timer);
        }//end else
    }//end if
}


void TA3_0_IRQHandler(void);
void TA3_0_IRQHandler(void){
    if(*(Timer0.Reg.CCTL) & Timer0.Reg.InterruptMask){
        completePartialRunTasks_interrupt(&Timer0);
    }
}


void TA3_N_IRQHandler(void);
void TA3_N_IRQHandler(void){
    //check if interrupt was caused by completing a full run
    if(TIMER_A3->CTL & TIMER_A_CTL_IFG){
        //handle timer overflow
        completeFullRunTasks_interrupt(&Timer0);
        completeFullRunTasks_interrupt(&Timer1);
        completeFullRunTasks_interrupt(&Timer2);
        completeFullRunTasks_interrupt(&Timer3);
        completeFullRunTasks_interrupt(&Timer4);



    }else{
        if(*(Timer1.Reg.CCTL) & Timer1.Reg.InterruptMask){
            completePartialRunTasks_interrupt(&Timer1);
        }
        if(*(Timer2.Reg.CCTL) & Timer2.Reg.InterruptMask){
            completePartialRunTasks_interrupt(&Timer2);

        }
        if(*(Timer3.Reg.CCTL) & Timer3.Reg.InterruptMask){
            completePartialRunTasks_interrupt(&Timer3);

        }
        if(*(Timer4.Reg.CCTL) & Timer4.Reg.InterruptMask){
            completePartialRunTasks_interrupt(&Timer4);

        }
    }
}
/*
 * full run interrupt when 0xFFFF->0x0000
 * TAIFG flag (bit 0) in TAxCTL register (0 no flag, 1, interrupt pending)
 * TAIE disables and enables interrupt (bit 1 in CTL)
 *
 */

//__enable_irq(); //enable global interrupt

