/*
 * Switches.h
 *
 *  Created on: Feb 16, 2024
 *      Author: zawaidhm
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_
#include "WateringTimer.h"
#include <stdbool.h> //true, false, bool


#define switchPort P4
#define LEDPort P5
#define plant0Mask 0b1
#define plant1Mask 0b10
#define plant2Mask 0b100
#define plant3Mask 0b1000
#define plant4Mask 0b10000
#define levelMask 0b100000
#define masterMask 0b1000000


#define DEBOUNCE_PERIOD 0.005 // ms
#define DEBOUNCE_LOOP_CYCLES 2
#define DEBOUNCE_COUNT DEBOUNCE_PERIOD*SMCLK/DEBOUNCE_LOOP_CYCLES //TODO check value
void debounce(void);


typedef struct{
    volatile bool _0isPressed;
    volatile bool _1isPressed;
    volatile bool _2isPressed;
    volatile bool _3isPressed;
    volatile bool _4isPressed;
    volatile bool levelIsLow;
    volatile bool masterIsPressed;
}SwitchStatuses;

bool checkSW0(void);
bool checkSW1(void);
bool checkSW2(void);
bool checkSW3(void);
bool checkSW4(void);
bool checkLevelSW(void);
bool checkMasterSW(void);

void checkAllSwitches(void);

void turnOffLED(uint16_t mask);
void turnOnLED(uint16_t mask);



#endif /* SWITCHES_H_ */
