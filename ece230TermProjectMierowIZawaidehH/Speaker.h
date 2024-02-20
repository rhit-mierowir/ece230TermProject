/*
 * Speaker.h
 *
 *  Created on: Feb 19, 2024
 *      Author: zawaidhm
 */

#ifndef SPEAKER_H_
#define SPEAKER_H_
/* DriverLib Includes */
#include <msp.h>

/* Standard Includes */
#include <stdio.h>  //NULL is defined in this header file
#include "csHFXT.h"
#include "csLFXT.h"
//48MHz system clock and 4:1 prescaler
//12MHz Timer A clock
//Half period count = (48,000,000/Note frequency)
#define SMCLK 48000000 //Hz
#define TimerA0Prescaler 4 //Timer A prescaler
#define TimerA0Clock  SMCLK/TimerA0Prescaler
#define SpeakerPort P2
#define Speaker BIT4  //P2.4

void PlayerConfiguration(void);
void PlayNote (unsigned int CurrentNote);
void  PlayAllNotesOnce(const uint16_t notelist[]);
#define NoteDuration 100000
void notifyUserLowWaterBuzzer(void);
//rest note, one shot signal of 1/48MHz pulse width is generated,
//which is not audible
//This is not the inserted rest for 100ms when Speaker should remain low voltage.
#define RestNote 1

#define FrequencyA4 440 //Hz
#define FrequencyB4 493.88 //Hz
#define FrequencyC5 523.26 //Hz

#define FrequencyG3 196
#define FrequencyD4 293.7
#define FrequencyE4 329.6
#define FrequencyC4 261.6
#define FrequencyB3 246.9
#define FrequencyA3 220

#define FrequencyF4s 369.99 //Lowest in song
#define FrequencyC5s 554.4
#define FrequencyD3 147
#define FrequencyE3 161.82
#define FrequencyE2 82
#define FrequencyF2s 92.5
#define FrequencyG2 98

#define FrequencyD5 587.33
#define FrequencyE5 659.25
#define FrequencyF5s 739.99
#define FrequencyG5 783.99 // highest in song



// Note A4 - 440 Hz, B4 - 493.88 Hz, C5 - 523.26 Hz
#define NOTEA4  TimerA0Clock/FrequencyA4    // (48,000,000/440)=4545.45
#define NOTEB4  TimerA0Clock/FrequencyB4   //4049.57
#define NOTEC5  TimerA0Clock/FrequencyC5   //3822.19
#define NOTEG3  TimerA0Clock/FrequencyG3
#define NOTED4  TimerA0Clock/FrequencyD4
#define NOTEE4  TimerA0Clock/FrequencyE4
#define NOTEC4  TimerA0Clock/FrequencyC4
#define NOTEB3  TimerA0Clock/FrequencyB3
#define NOTEA3  TimerA0Clock/FrequencyA3
#define NOTEF4s  TimerA0Clock/FrequencyF4s
#define NOTEC5s  TimerA0Clock/FrequencyC5s
#define NOTED3  TimerA0Clock/FrequencyD3
#define NOTEE3  TimerA0Clock/FrequencyE3
#define NOTEE2  TimerA0Clock/FrequencyE2
#define NOTEF2s  TimerA0Clock/FrequencyF2s
#define NOTEG2  TimerA0Clock/FrequencyG2

#define NOTED5  TimerA0Clock/FrequencyD5
#define NOTEE5  TimerA0Clock/FrequencyE5
#define NOTEF5s  TimerA0Clock/FrequencyF5s
#define NOTEG5  TimerA0Clock/FrequencyG5

#endif /* SPEAKER_H_ */
