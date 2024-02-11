/*
 * Pump.h
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */

#ifndef PUMP_H_
#define PUMP_H_

#include <msp.h>

/*
 *
 */
typedef struct{
    volatile _Bool IsActive;
    const uint16_t Mask;
}PumpInfo;

#define PUMP_PORT P5
#define PUMP_0_MASK 0b1 << 0
#define PUMP_1_MASK 0b1 << 1
#define PUMP_2_MASK 0b1 << 2
#define PUMP_3_MASK 0b1 << 3
#define PUMP_4_MASK 0b1 << 4




//Initialize all 5 pumps within this class (see below function), use extern to grab individual pumps
void initAllPumps();

// Act on individual pumps.
void initPump(PumpInfo *pump);
void togglePump(PumpInfo *pump);
void turnOffPump(PumpInfo *pump);
void turnOnPump(PumpInfo *pump);

#endif /* PUMP_H_ */
