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
typedef struct {
    unsigned int fullRunCount;
    unsigned short finalRunTicks;
}TimerTicks;


void initWateringTimer(void);



