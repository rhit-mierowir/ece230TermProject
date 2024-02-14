/*
 * Time.h
 *
 *  Created on: Feb 4, 2024
 *      Author: zawaidhm
 */

#ifndef TIME_H_
#define TIME_H_

typedef struct {
    unsigned short day;
    unsigned char hr;
    unsigned char min;
    unsigned char sec;
    unsigned short ms;
}TimeLength;

typedef enum {day,hr,min,sec,ms}TimeDesignator;

void stringToTime(char *inputStr, TimeLength *time);
void timeToString (char *Buffer, TimeLength *time, _Bool addFiller);

//This is currently more of an internal function. might be uesful.
void addTimeRollover(TimeLength *time, unsigned long count, TimeDesignator td);

#endif /* TIME_H_ */
