/*
 * Procedures.c
 *
 *  Created on: Feb 17, 2024
 *      Author: zawaidhm
 */

#include "Pump.h"
#include "WateringTimer.h"
#include "Procedures.h"
#include <stdio.h> //sprintf
#include <Switches_LEDs.h>


extern PumpInfo Pump0, Pump1, Pump2, Pump3, Pump4;
extern TimerData Timer0, Timer1, Timer2, Timer3, Timer4;


void pausePump(PumpInfo *pump){
    pump->storedStateIsActive=pump->IsActive;
    turnOffPump(pump);

}

void resumePump(PumpInfo *pump){
    if(pump->storedStateIsActive){
        turnOnPump(pump);
    }
}

void pauseFunctions(void){
    //pause Timer
    pauseTimer();
    //need to store state of pumps before pausing
    //stop any active pumps
    pausePump(&Pump0);
    pausePump(&Pump1);
    pausePump(&Pump2);
    pausePump(&Pump3);
    pausePump(&Pump4);

}

void restoreFunctions(void){
    //retrieve the state of the pumps from before pausing
    //restore pumps to previous status
    //resume timer
    resumePump(&Pump0);
    resumePump(&Pump1);
    resumePump(&Pump2);
    resumePump(&Pump3);
    resumePump(&Pump4);
    resumeTimer();
}


void startNewWaterCycle(TimerData *timer){
    turnOffPump(timer->Pump);
    startTimerCycle_interrupt(timer);
}


void plantButtonTasks(_Bool pressed, TimerData *timer){
    if(pressed){
        startNewWaterCycle(timer);
    }
}

void performAllPlantButtonsTasks(void){
    plantButtonTasks(checkSW0(),&Timer0);
    plantButtonTasks(checkSW1(),&Timer1);
    plantButtonTasks(checkSW2(),&Timer2);
    plantButtonTasks(checkSW3(),&Timer3);
    plantButtonTasks(checkSW4(),&Timer4);

}

volatile _Bool masterSwitchCurrentStatus=0;
void masterSwitchTasks(void){
    if(checkMasterSW()){
        masterSwitchCurrentStatus=!(masterSwitchCurrentStatus);
        if(masterSwitchCurrentStatus){
            turnOnLED(ledMasterMask);
            pauseFunctions();
        }else{
            turnOffLED(ledMasterMask);
            restoreFunctions();
        }
    }
}

void levelSwitchTasks(void){
    if(checkLevelSW()){ //pressed
        turnOnLED(ledLevelMask);
    }else{
        turnOffLED(ledLevelMask);
    }
}


//takes a timer object and a string with time value that we want to set
/*
 * Receives a timer and a string with a new time for watering
 * calls stringToTime to convert the string to a TimeLength object and update the WateringLength object within the Timer
 * updates the timer settings for the Timer based on this new value
 */
void setWaterTime(TimerData *timer, char *inputStr);
void setWaterTime(TimerData *timer, char *inputStr){
    stringToTime(inputStr, &(timer->TimerTimes.WateringLength));
    updateTimerTickSettings(timer);
}

void setWaitTime(TimerData *timer, char *inputStr);
void setWaitTime(TimerData *timer, char *inputStr){
    stringToTime(inputStr, &(timer->TimerTimes.WaitLength));
    updateTimerTickSettings(timer);
}

//TODO printSettings - should return a string
//returnTimerSettings();

//TODO printToUser
void displayTimerSettings(TimerData *timer);

