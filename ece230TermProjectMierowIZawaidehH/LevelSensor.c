/*
 * LevelSensor.c
 *
 *  Created on: Feb 15, 2024
 *      Author: zawaidhm
 */
#include "LevelSensor.h"
#include <msp.h>
#include <stdbool.h> //true, false, bool

bool waterIsLow = false;

void configureLevelSensor()
{
    //Set input, DIR=0
    WATER_SENSOR_PORT->DIR &= ~WATER_SENSOR_MASK;

    //Set to default GPIO SEL=0,0
    WATER_SENSOR_PORT->SEL0 &= ~WATER_SENSOR_MASK;
    WATER_SENSOR_PORT->SEL1 &= ~WATER_SENSOR_MASK;

    //Set to enable pullup resistor REN=1
    WATER_SENSOR_PORT->REN |= WATER_SENSOR_MASK;

    //Set to pullup resistor OUT=1
    WATER_SENSOR_PORT->OUT |= WATER_SENSOR_MASK;
}
bool readWaterLevelSensor()
{
    //Read the Sensor and condition on it to determine if the
    // If pin is high, switch not pressed
    if ( WATER_SENSOR_PORT->IN & WATER_SENSOR_MASK ) {
        //waterLevel.IsLow=0;
        waterIsLow = false;
        waterLevelGood();
    } else {
        waterIsLow = true;
        waterLevelLow();
    }
    return waterIsLow;
}

void waterLevelLow()
{
    //Turn on LED1
    notif_LED_PORT->OUT |= notif_LED_MASK;

    //Other things to do when the water level is low
}
void waterLevelGood()
{
    //Turn off LED1
    notif_LED_PORT->OUT &= ~notif_LED_MASK;

    //Other things to do when the water level is good
}


