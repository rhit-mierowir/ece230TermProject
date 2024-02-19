/*
 * Procedures.h
 *
 *  Created on: Feb 17, 2024
 *      Author: zawaidhm
 */

#ifndef PROCEDURES_H_
#define PROCEDURES_H_
#include "WateringTimer.h"


void pausePump(PumpInfo *pump);
void resumePump(PumpInfo *pump);
void pauseFunctions(void);
void restoreFunctions(void);
void startNewWaterCycle(TimerData *timer);
void levelSwitchTasks(void);
void masterSwitchTasks(void);



void setWaterTime(TimerData *timer, char *inputStr);
void setWaitTime(TimerData *timer, char *inputStr);


#endif /* PROCEDURES_H_ */
