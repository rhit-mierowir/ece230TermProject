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








//code goes above endif statement
#endif /* WATERINGTIMER_H_ */
