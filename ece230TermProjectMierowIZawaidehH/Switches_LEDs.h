/*
 * Switches.h
 *
 *  Created on: Feb 16, 2024
 *      Author: zawaidhm
 */

#ifndef SWITCHES_LEDS_H_
#define SWITCHES_LEDS_H_
#include "WateringTimer.h"
#include <stdbool.h> //true, false, bool




#define led0Mask 0b1 //pin 0
#define led1Mask 0b10 //pin 1
#define led2Mask 0b100 //pin 2
//edited to reflect P5.3 not being available
#define led3Mask 0b10000 //pin4
#define led4Mask 0b100000 //pin 5
#define ledMasterMask 0b1000000 //pin 6
#define ledLevelMask 0b10000000 //pin 7
#define LEDPort P5
void turnOffLED(uint16_t mask);
void turnOnLED(uint16_t mask);
void configureAllLEDs(void);

#define DEBOUNCE_PERIOD 0.005 // ms
#define DEBOUNCE_LOOP_CYCLES 2
#define DEBOUNCE_COUNT DEBOUNCE_PERIOD*SMCLK/DEBOUNCE_LOOP_CYCLES //TODO check value
void debounce(void);

#define switchPort P4
#define sw0Mask 0b1
#define sw1Mask 0b10
#define sw2Mask 0b100
#define sw3Mask 0b1000
#define sw4Mask 0b10000
#define levelSWMask 0b100000
#define masterSWMask 0b1000000

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
void configureAllPlantSwitches(void);
void configureSwitches_LEDs(void);




#endif /* SWITCHES_LEDS_H_ */
