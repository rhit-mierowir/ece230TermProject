/*
 * WateringTimer.c
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */

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

typedef struct{
    unsigned volatile int fullRunsRemaining; //variable that counts down from fullRunCount and holds number of full runs left
    unsigned volatile short finalRunTicks; // stores the number that we are comparing against in our final run to know when to interrupt the timer
}TimerValues;

typedef struct {
    TimerSettings WateringSettings;
    TimerSettings WaitingSettings;
    TimerValues ActiveValues;
}TimerInformation;



void initWateringTimer(void);



