#include "Communication.h"

#include "time.h"
#include "uart_routines2024.h"
#include "WateringTimer.h"
#include <stdio.h> //sprintf(), strcmp();
#include <stdbool.h> //true, false, bool
#include <string.h> //strlen(),


/*
 * Commands:
 *
 * WTR - water a plant, then restart delay
 *  | Which Plant? [0-4]
 * STP - stop the timer, turn off motors.
 * TME - Set the time of a timer.
 *  | Which Timer? [0-4]
 *  | (W)atering Time or (D)elay time?
 *  | How Long? (input a time string. e.g. "1d 5h 3m 2s 1ms")
 * EMP - Set Action When Empty.
 *  | What indicator do you want shown when water is empty (can select multiple)? (0) None, (1) Turn on LED, (2) Sound Buzzer
 *  | Should Timer be stopped while low? (Y)es, (N)o
 *  > Y
 *    | Should we return after stop by (W)atering, (D)elaying, or (R)eturning to delay in progress.
 * SVE - Save Timer Configurations
 * RST - Reset by loading all timers from Configurations
 * PNT - Print all settings for the timers and sensors
 */
#define COMMAND_LENGTH 3
#define CLEAR_CHAR ' '
// number subtracted to lowercase numbers to make uppercase
#define LOWERCASE_TO_UPPERCASE_SHIFT  0x20

//Declare all of the command strings
#define CMD_WaterPlant              "WTR"
#define CMD_StopTimer               "STP"
#define CMD_SetTimerLength          "TME"
#define CMD_DecideActionWhenEmpty   "EMP"
#define CMD_SaveTimerConfig         "SVE"
#define CMD_ResetToTimerConfig      "RST"
#define CMD_PrintAllToScreen        "PNT"

//This is the message sent if the command was not found. %s replaced by command received.
#define StringIfCommandNotRecognized "Command \"%s\" not recognized."
// length of the above string, but it removes the 2chars '%s', replacing with length of command
#define StringIfCommandNotRecognizedLength 31
//(strlen(StringIfCommandNotRecognized) -2 + COMMAND_LENGTH)


char CommandBuffer[COMMAND_LENGTH + 1]; //holds 3 characters for commands (+1 for null)
unsigned short CommandBufferIndex = 0; // the index of the next character to add

// this is how we specify what string to send
void sendString(char *Buffer);
void sendString(char *Buffer){
    printf(Buffer);
    printf("\r\n");
}

void evaluateCommandBuffer(void);
void evaluateCommandBuffer(void){
    //If we are evaluating a full command
    if(CommandBufferIndex >= COMMAND_LENGTH){
        //Check if it is a valid command
        if (strcmp(CommandBuffer,CMD_WaterPlant)==0){
            performWaterPlant();
            clearCommandBuffer();
            return;

        }else if(strcmp(CommandBuffer,CMD_StopTimer)==0){
            performStopTimer();
            clearCommandBuffer();
            return;


        }else if(strcmp(CommandBuffer,CMD_SetTimerLength)==0){
            setTimerLength();
            clearCommandBuffer();
            return;


        }else if(strcmp(CommandBuffer,CMD_DecideActionWhenEmpty)==0){
            decideActionWhenEmpty();
            clearCommandBuffer();
            return;


        }else if(strcmp(CommandBuffer,CMD_SaveTimerConfig)==0){
            saveTimerConfig();
            clearCommandBuffer();
            return;


        }else if(strcmp(CommandBuffer,CMD_ResetToTimerConfig)==0){
            resetToTimerConfig();
            clearCommandBuffer();
            return;


        }else if(strcmp(CommandBuffer,CMD_PrintAllToScreen)==0){
            printAllToScreen();
            clearCommandBuffer();
            return;


        }
        //continue adding valid commands here.
    }

    //inform that no command was recognized
    char* buffer[StringIfCommandNotRecognizedLength];
    sendString(sprintf(buffer,StringIfCommandNotRecognized,CommandBuffer));


    clearCommandBuffer(); //clear commands when finished
    return;
}

/*
 * This initializes the communication module. Run this prior to using.
 */
void initCommunication();
void initCommunication(){
    clearCommandBuffer();//reset the command buffer
}

/**
 * Clear the command Buffer by fully populating with clear Chars, reset CommmandBufferIndex to 0
 */
void clearCommandBuffer();
void clearCommandBuffer(){
    int i;
    for (i = 0; i < COMMAND_LENGTH;i++){
        CommandBuffer[i] = CLEAR_CHAR;
    }
    CommandBufferIndex = 0;
}

/*
 * This adds the character to the command buffer and evaluates the command if the enter key is pressed.
 * The Command buffer only accepts letters and numbers, capitalizes all letters
 * Returns true if an update occurred, changing the value of CommandBuffer
 */
bool addCharToCommandBuffer(char addChar);
bool addCharToCommandBuffer(char addChar){

    if (CommandBufferIndex >= COMMAND_LENGTH){
        //if passed last character
        switch(addChar){
            case '\b': //backspace
            case 0x7F: //delete
                //go back that character
                CommandBufferIndex--;
                //Clear That Character
                CommandBuffer[CommandBufferIndex] = CLEAR_CHAR;
                return true;
            case '\n': //enter Pressed
                evaluateCommandBuffer();
                return true;
            default:
                //no action
                return false;
            }
    }

        //if at a valid index to insert char
        switch(addChar){
        case '\b': //backspace
        case 0x7F: //delete
            //if no character to remove, ignore this command
            if (CommandBufferIndex == 0) {break;}

            //go back one character
            CommandBufferIndex--;
            //Clear That Character
            CommandBuffer[CommandBufferIndex] = CLEAR_CHAR;
            return true;
        case '\n': // hit enter
            evaluateCommandBuffer();
            return true;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            //make this an upper case letter, and drop to uppercase letter evaluation to add it
            addChar -= LOWERCASE_TO_UPPERCASE_SHIFT;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            // add Capital letters character, and increment buffer index
            // drop down past numbers to add to buffer
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
            // add numbers and increment buffer index
            CommandBuffer[CommandBufferIndex] = addChar;
            CommandBufferIndex++;
            return true;
        default:
            //ignore other characters
            return false;

        }
}

/* This command picks which plant to water and alters the timer to make that happen
 * WTR - water a plant, then restart delay
 *  | Which Plant? [0-4]
 */
void performWaterPlant(void);
void performWaterPlant(void){
    sendString("Implement performWarterPlant");
}

/*
 * This command simply stops the timer so no time change occours. This also turns off any active motors.
 * STP - stop the timer, turn off motors.
 */
void performStopTimer(void);
void performStopTimer(void){
    sendString("Implement performStopTimer");
}

/*
 * TME - Set the time of a timer.
 *  | Which Timer? [0-4]
 *  | (W)atering Time or (D)elay time?
 *  | How Long? (input a time string. e.g. "1d 5h 3m 2s 1ms")
 */
void setTimerLength(void);
void setTimerLength(void){
    sendString("Implement setTimerLength");
}

/*
 * EMP - Set Action When Empty.
 *  | What indicator do you want shown when water is empty (can select multiple)? (0) None, (1) Turn on LED, (2) Sound Buzzer
 *  | Should Timer be stopped while low? (Y)es, (N)o
 *  > Y
 *    | Should we return after stop by (W)atering, (D)elaying, or (R)eturning to delay in progress.
 */
void decideActionWhenEmpty(void);
void decideActionWhenEmpty(void){
    sendString("Implement decideActionWhenEmpty");
}

/*
 * SVE - Save Timer Configurations
 */
void saveTimerConfig(void);
void saveTimerConfig(void){
    sendString("Implement saveTimerConfig");
}

/*
 * RST - Reset by loading all timers from Configurations
 */
void resetToTimerConfig(void);
void resetToTimerConfig(void){
    sendString("Implement resetToTimerConfig");
}

/*
 *
 */
void printAllToScreen(void);
void printAllToScreen(void){
    sendString("Implement printAllToScreen");
}
