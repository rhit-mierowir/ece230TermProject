/*
 * Pump.c
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */


typedef struct{
    volatile _Bool IsActive;
    uint16_t *Pin;
    uint16_t Mask;
}PumpInfo;

//Initialize all 5 pumps within this class (see below function), use extern
public void initializePump();

public void togglePump(PumpInfo *pump);
public void turnOffPump(PumpInfo *pump);
public void turnOnPump(PumpInfo *pump);


