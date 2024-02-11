/*
 * Time.c
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */

#include "Time.h"
#include <stdio.h>

//typedef struct {
//    unsigned short day;
//    unsigned char hr;
//    unsigned char min;
//    unsigned char sec;
//    unsigned short ms;
//}TimeLength;

TimeLength stringToTime(String inputTime);

char Buffer[20];

void timeToString (TimeLength inputTime, ){
    char timeStr[20];
    sprintf(timeStr, "%dd%dh%dm%s%ms",TimeLength.day,TimeLength.hr,TimeLength.min,TimeLength.sec,TimeLength.ms);
    return timeStr;
}

