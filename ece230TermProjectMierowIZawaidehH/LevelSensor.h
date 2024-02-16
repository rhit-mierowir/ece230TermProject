/*
 * LevelSensor.h
 *
 *  Created on: Feb 15, 2024
 *      Author: zawaidhm
 */


#ifndef LEVELSENSOR_H_
#define LEVELSENSOR_H_
#include <stdbool.h>
//Water Level Sensor Constants P4.5
#define WATER_SENSOR_PORT P4
#define WATER_SENSOR_MASK 0b100000
//TODO determine circuit schematic wiring and set these accordingly
#define notif_LED_PORT P5
#define notif_LED_MASK 0b100000

void configureLevelSensor();
bool readWaterLevelSensor();
void waterLevelLow();
void waterLevelGood();


#endif /* LEVELSENSOR_H_ */
