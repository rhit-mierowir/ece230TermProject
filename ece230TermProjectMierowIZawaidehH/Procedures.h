/*
 * Procedures.h
 *
 *  Created on: Feb 17, 2024
 *      Author: zawaidhm
 */

#ifndef PROCEDURES_H_
#define PROCEDURES_H_
#include "WateringTimer.h"

#define led0Mask 0b1
#define led1Mask 0b10
#define led2Mask 0b100
#define led3Mask 0b1000
#define led4Mask 0b10000
#define ledLevelMask 0b100000
#define ledMasterMask 0b1000000

#define LEDPort P5
void pausePump(PumpInfo *pump);
void resumePump(PumpInfo *pump);
void pauseFunctions(void);
void restoreFunctions(void);
void startNewWaterCycle(TimerData *timer);
void levelSwitchTasks(void);
void masterSwitchTasks(void);

void turnOffLED(uint16_t mask);
void turnOnLED(uint16_t mask);

void setWaterTime(TimerData *timer, char *inputStr);
void setWaitTime(TimerData *timer, char *inputStr);


#endif /* PROCEDURES_H_ */
