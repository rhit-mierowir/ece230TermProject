/*
 * Pump.c
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */
#include "Pump.h"
#include <msp.h>

//typedef struct{
//    volatile _Bool IsActive;
//    uint16_t *Port;
//    uint16_t Mask;
//}PumpInfo;


PumpInfo Pump0 = {
    .IsActive = 0,
    .Mask = PUMP_0_MASK
};
PumpInfo Pump1 = {
    .IsActive = 0,
    .Mask = PUMP_1_MASK
};
PumpInfo Pump2 = {
    .IsActive = 0,
    .Mask = PUMP_2_MASK
};
PumpInfo Pump3 = {
    .IsActive = 0,
    .Mask = PUMP_3_MASK
};
PumpInfo Pump4 = {
    .IsActive = 0,
    .Mask = PUMP_4_MASK
};

//This list stores all pumps, will be used to initializeAllPumps
PumpInfo* allPumps[] = {&Pump0, &Pump1, &Pump2, &Pump3, &Pump4};
int pumpCount = sizeof(allPumps)/sizeof(allPumps[0]);


// uses allPumps[] list to initialize all pumps it contains
void initAllPumps(){
    int i;
    for(i=0; i<pumpCount;i++){
        initializePump((allPumps[i]));
    }
}

void initPump(PumpInfo *pump){
    PUMP_PORT->DIR &= ~(pump->Mask); //set direction to 0 - output
    PUMP_PORT->SEL0 &= ~(pump->Mask); //set function select to 00 - I/O
    PUMP_PORT->SEL1 &= ~(pump->Mask); //set function select to 00 - I/O

    //Default to pumps off.
    PUMP_PORT->OUT &= ~(pump->Mask);
    pump->IsActive = 0;
}

void togglePump(PumpInfo *pump){
    if (pump->IsActive){
        turnOffPump(pump);
    } else {
        turnOnPump(pump);
    }
}

void turnOffPump(PumpInfo *pump){
    pump->IsActive = 0;
    PUMP_PORT->OUT &= ~(pump->Mask);
}

void turnOnPump(PumpInfo *pump){
    pump->IsActive = 1;
    PUMP_PORT->OUT |= (pump->Mask);
}
