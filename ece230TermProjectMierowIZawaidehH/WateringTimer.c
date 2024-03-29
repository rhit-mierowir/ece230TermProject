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
#include "Procedures.h"

extern PumpInfo Pump0, Pump1, Pump2, Pump3, Pump4;

//Declare structs for all timers
TimerData Timer0 = {
                    .Reg = {
                        .CCR = &(Timer0CCR),
                        .CCTL = &(Timer0CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
                    //let timer times be undeclared.
                    .Pump = &Pump0
};
TimerData Timer1 = {
                    .Reg = {
                        .CCR = &(Timer1CCR),
                        .CCTL = &(Timer1CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
                        .Pump = &Pump1

};
TimerData Timer2 = {
                    .Reg = {
                        .CCR = &(Timer2CCR),
                        .CCTL = &(Timer2CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
                        .Pump = &Pump2

};
TimerData Timer3 = {
                    .Reg = {
                        .CCR = &(Timer3CCR),
                        .CCTL = &(Timer3CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
                        .Pump = &Pump3

};
TimerData Timer4 = {
                    .Reg = {
                        .CCR = &(Timer4CCR),
                        .CCTL = &(Timer4CCTL),
                        .InterruptMask = interruptMask,
                        .InterruptEnableMask=interruptEnableMask
                        },
                        .Pump = &Pump4

};

void pauseTimer(){
    //bits 4 and 5 of TAxCTL
    //00b - stop mode
    //10b - continuous mode
    WTimer->CTL &=~WTimerModeContinuousBit;
}
void resumeTimer(){
    //bits 4 and 5 of TAxCTL
    //00b - stop mode
    //10b - continuous mode
    WTimer->CTL |=WTimerModeContinuousBit;
}


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
//    settingToChange->additionalTicks = totalTicks%(2^16);
//    settingToChange->fullRunCount = (totalTicks-settingToChange->additionalTicks)/(2^16);
    settingToChange->additionalTicks = totalTicks%(65536);
    settingToChange->fullRunCount = (totalTicks-settingToChange->additionalTicks)/(65536);

}


/*
 * Updates the number of ticks for waiting and watering.
 * Should be called when the TimeLength is updated
 */
void updateTimerTickSettings(TimerData *timer){
    convertTimerLengthToTicks(&(timer->TimerTimes.WateringLength),&(timer->WateringSettings));
    convertTimerLengthToTicks(&(timer->TimerTimes.WaitLength), &(timer->WaitingSettings));
    pauseTimer();
    if(timer->Pump->IsActive){
        recalculateActiveValues((&timer->WateringSettings), &(timer->ActiveValues));

    }else{
        recalculateActiveValues(&(timer->WaitingSettings), &(timer->ActiveValues));
    }
    if(timer->ActiveValues.fullRunsRemaining==0){
        initFinalRun_interrupt(timer);
    }
    resumeTimer();
}



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

    NVIC->ISER[0] |= (1)<<TA3_0_IRQn;
    NVIC->ISER[0] |= (1)<<TA3_N_IRQn;

    __enable_irq(); //enable global interrupt


}


/*
 * using the current timer location, use the proper offset for desired time length, be able to refer to correct register to get current number of ticks
 */
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

void initFinalRun_interrupt(TimerData *timer){
    //clear the interrupt flag.
    *(timer->Reg.CCTL) &= ~(timer->Reg.InterruptMask);
    //enable TAxCCTLy CCIFG for the particular pump
    *(timer->Reg.CCTL) |= timer->Reg.InterruptEnableMask;


    //TAxCCRy = finalRunTicks
    *(timer->Reg.CCR) =timer->ActiveValues.finalRunTicks;
}




//concludes the final run (partial cycle),
void completePartialRunTasks_interrupt(TimerData *timer){
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
void completeFullRunTasks_interrupt(TimerData *timer){
    if(timer->ActiveValues.fullRunsRemaining!=0){
        timer->ActiveValues.fullRunsRemaining--;
    }
    if(timer->ActiveValues.fullRunsRemaining==0){
        pauseTimer();
        if((timer->ActiveValues.finalRunTicks)<= DELTA){
            startTimerCycle_interrupt(timer);
        }else{
            initFinalRun_interrupt(timer);
        }//end else
        resumeTimer();
    }//end if
}


void TA3_0_IRQHandler(void){
    if(*(Timer0.Reg.CCTL) & Timer0.Reg.InterruptMask){
        *(Timer0.Reg.CCTL) &= ~(Timer0.Reg.InterruptMask);// clear interrupt flag
        completePartialRunTasks_interrupt(&Timer0);
    }
}


void TA3_N_IRQHandler(void){
    //check if interrupt was caused by completing a full run
    if(TIMER_A3->CTL & TIMER_A_CTL_IFG){
        //handle timer overflow
        completeFullRunTasks_interrupt(&Timer0);
        completeFullRunTasks_interrupt(&Timer1);
        completeFullRunTasks_interrupt(&Timer2);
        completeFullRunTasks_interrupt(&Timer3);
        completeFullRunTasks_interrupt(&Timer4);
        TIMER_A3->CTL &=~TIMER_A_CTL_IFG; //clear interrupt



    }
    if(*(Timer1.Reg.CCTL) & TIMER_A_CCTLN_CCIFG){
        *(Timer1.Reg.CCTL) &= ~(Timer1.Reg.InterruptMask);// clear interrupt flag
        completePartialRunTasks_interrupt(&Timer1);
    }
    if(*(Timer2.Reg.CCTL) & Timer2.Reg.InterruptMask){
        *(Timer2.Reg.CCTL) &= ~(Timer2.Reg.InterruptMask);// clear interrupt flag
        completePartialRunTasks_interrupt(&Timer2);

    }
    if(*(Timer3.Reg.CCTL) & Timer3.Reg.InterruptMask){
        *(Timer3.Reg.CCTL) &= ~(Timer3.Reg.InterruptMask);// clear interrupt flag
        completePartialRunTasks_interrupt(&Timer3);

    }
    if(*(Timer4.Reg.CCTL) & Timer4.Reg.InterruptMask){
        *(Timer4.Reg.CCTL) &= ~(Timer4.Reg.InterruptMask);// clear interrupt flag
        completePartialRunTasks_interrupt(&Timer4);

    }

}
/*
 * full run interrupt when 0xFFFF->0x0000
 * TAIFG flag (bit 0) in TAxCTL register (0 no flag, 1, interrupt pending)
 * TAIE disables and enables interrupt (bit 1 in CTL)
 *
 */


