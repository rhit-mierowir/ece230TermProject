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


/* addTimeRollover(TimeLength *time, unsigned long count, TimeDesignator td)
 * *time: Adds time to the time object pointed to by *time
 * count: adds the number of the time increment specified by count
 * td:    the time increment needed
 */
void addTimeRollover(TimeLength *time, unsigned long count, TimeDesignator td){
    if (count == 0){
        return; //return if there is no time change.
    }
    switch(td){
    case day:
        time->day += count;
        return;
    case hr:
        count += time->hr;
        time->hr = count % 24; //24 hr = a day
        addTimeRollover(time,count/24,day); // integer division give remaining number of time
        return;
    case min:
        count += time->min;
        time->min = count % 60; //60 mins = 1hr
        addTimeRollover(time,count/60,hr);//int div, store hours
        return;
    case sec:
        count += time->sec;
        time->sec = count % 60; //60 sec = 1 min
        addTimeRollover(time,count/60,min);
        return;
    case ms:
        count += time->ms;
        time->ms = count % 1000; // 1000 ms = 1 sec
        addTimeRollover(time,count/1000,sec);
        return;
    default:
        return;
    }
}

/* This buffer must have room for at least 25 characters including null
 *
 * "5d 2h 30m 20s 380ms"      - addFiller=0
 * "    5d  2h 30m 20s 380ms" - addFiller=1
 * "_____d __h __m __s ___ms" (25 chars including null)
 */
void timeToString (char *Buffer, TimeLength *time, _Bool addFiller){
    if (addFiller){
        // Write characters with filler, fill with spaces
        // "    5d  2h 30m 20s 380ms"
        sprintf(Buffer, "%5ud %2uh %2um %2us %3ums",time->day,time->hr,time->min,time->sec,time->ms);
    } else {
        // Write characters without filler
        // "5d 2h 30m 20s 380ms"
        sprintf(Buffer, "%ud %uh %um %us %ums",time->day,time->hr,time->min,time->sec,time->ms);
    }

}

/* stringToTime(char *inputStr, TimeLength *time)
 * The TimeLength object is cleared (set to 0), then
 * uses the inputStr to fill in a TimeLength object.
 *
 * Format: "5d 2h 30m 20s 380ms"
 * Time Designators: d  - days
 *                   h  - hours
 *                   m  - minutes
 *                   s  - seconds
 *                   ms - milliseconds
 * The number prefixed to these time designators indicate
 * the number of them to store.
 * These prefixes don't have to be given in a specific order.
 * All non-number and non-Time Designator characters are ignored.
 * If no time prefixes a designator, it assumes 0.
 * If no time designator is given after it, it ignores the number.
 * If there are multiple instances of a time designator, it adds to the previous values assigned.
 */
void stringToTime(char *inputStr, TimeLength *time){
    //clear time
    time->day = 0;
    time->hr = 0;
    time->min = 0;
    time->sec = 0;
    time->ms = 0;

    //Used to build numbers before stored in time
    unsigned long numBuilding= 0;
    //Used to check for ms time designator
    _Bool checkForMS = 0;

    int i = 0;
    //loop through entire string
    while (1){
        switch(inputStr[i]){

        //stop when reach end of string
        case '\0':
            if (checkForMS){
                //the 'm' character not part of 'ms'
                addTimeRollover(time,numBuilding, min);
            }
            return;
            break;

        // lookout for time specifier.
        // assign the number built to the specified time value.
        // restart number built at 0
        case 'd':
            if (checkForMS){
                //the 'm' character not part of 'ms'
                addTimeRollover(time,numBuilding, min);
                checkForMS = 0; numBuilding = 0;
            }
            addTimeRollover(time,numBuilding, day);
            numBuilding = 0;
            break;
        case 'h':
            if (checkForMS){
                //the 'm' character not part of 'ms'
                addTimeRollover(time,numBuilding, min);
                checkForMS = 0; numBuilding = 0;
            }
            addTimeRollover(time,numBuilding, hr);
            numBuilding = 0;
            break;
        case 'm':
            if (checkForMS){
                //the 'm' character not part of 'ms'
                addTimeRollover(time,numBuilding, min);
                checkForMS = 0; numBuilding = 0;
            }
            // next loop figure out if this is 'm' or start of 'ms'
            checkForMS = 1;
            break;
        case 's':
            if (checkForMS){
                // time designator was 'ms'
                addTimeRollover(time,numBuilding, ms);
            } else {
                // time designator was 's'
                addTimeRollover(time,numBuilding, sec);
            }
            checkForMS = 0;
            numBuilding = 0;
            break;

        // If a number, shift numBuilding left in base 10 and add number.
        // For numbers, their least significant byte is their value
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            // we also need to do checkForMS, incase '2m532s ...'
            if (checkForMS){
                //the 'm' character not part of 'ms'
                addTimeRollover(time,numBuilding, min);
                numBuilding = 0;
                checkForMS = 0;
            }
            numBuilding = numBuilding*10 + (inputStr[i] & 0xF);
            break;

        //nothing special recognized
        default:
            if (checkForMS){
                //the 'm' character not part of 'ms'
                addTimeRollover(time,numBuilding, min);
                numBuilding = 0;
                checkForMS = 0;
            }

            break;
        }

        i++; //go to next character
    }
}
