/*
 * LevelSensor.h
 *
 *  Created on: Feb 15, 2024
 *      Author: zawaidhm
 */


#ifndef LEVELSENSOR_H_
#define LEVELSENSOR_H_
#include <stdbool.h>
//Water Level Sensor Constants P1.5
#define WATER_SENSOR_PORT P1
#define WATER_SENSOR_MASK 0b100000
void configureLevelSensor();
bool readWaterLevelSensor();
void waterLevelLow();
void waterLevelGood();


#endif /* LEVELSENSOR_H_ */
