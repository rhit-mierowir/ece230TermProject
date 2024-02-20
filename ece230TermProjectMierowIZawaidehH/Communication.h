/*
 * Communication.h
 *
 *  Created on: Feb 4, 2024
 *      Author: mierowir
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_
#include <stdbool.h> //true, false, bool

#define COMMAND_LENGTH 3
#define CLEAR_CHAR ' '
#define NULL_CHAR  0
// number subtracted to lowercase numbers to make uppercase
#define LOWERCASE_TO_UPPERCASE_SHIFT  0x20

void evaluateCommandBuffer(void);
void clearCommandBuffer();
bool addCharToCommandBuffer(char addChar);

void displayWaterPlant(void);
void startWaterPlant(void);
void addCharWaterPlant(char nextChar);
void completeWaterPlant(char timerSelection);

void displaySetTimerLength(void);
void startSetTimerLength(void);
void addCharSetTimerLength(char nextChar);
void completeSetTimerLength(char waterOrDelay, char timerSelection, char *TimeSetting);

//The
#define NextLine "\r\n"
#define NewLine '\n'
#define Return '\r'
#define CommandEndChar Return
//The max length of strings for user input
#define UserInputBufferLength 200

//Declare all of the command strings
#define CMD_WaterPlant              "WTR"
#define CMD_StopTimer               "STP"
#define CMD_SetTimerLength          "TME"
#define CMD_DecideActionWhenEmpty   "EMP"
#define CMD_SaveTimerConfig         "SVE"
#define CMD_ResetToTimerConfig      "RST"
#define CMD_PrintAllToScreen        "PNT"


void sendString(char *Buffer);
void sendStringAndNewLine(char *Buffer);
bool storeInLastCharUntilEnter(char nextChar);
void clearLastCharBeforeEnter();
bool storeInUserInputBuffer(char nextChar);
void clearUserInputBuffer();
void displayLastCharPrompt(char *Prompt);
void displayUserInputBuffer(char *Prompt);
void displayCommandBuffer();
void initCommunication();
void displayCommunication(void);
void recieveCharForCommunication(char recieved);


#endif /* COMMUNICATION_H_ */
