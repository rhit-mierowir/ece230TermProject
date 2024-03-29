#include "Communication.h"
#include "Procedures.h"
#include "Time.h"
#include "uart_routines2024.h"
#include "WateringTimer.h"
#include <stdio.h> //sprintf(), strcmp();
#include <stdbool.h> //true, false, bool
#include <string.h> //strlen(),
#include"csHFXT.h"


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


extern TimerData Timer0, Timer1, Timer2, Timer3, Timer4;

enum CommunicationState{  Command,
                          WaterPlant,
                          StopTimer,
                          SetTimerLength,
                          DecideActionWhenEmpty,
                          SaveTimerConfig,
                          ResetToTimerConfig,
                          PrintAllToScreen
}ActiveState;

bool    displayMessage = true; //true if need to display message again

char    lastCharBeforeEnter = NULL_CHAR; //this is the last char

char    UserInputBuffer[UserInputBufferLength+1];//make room for null character at end
int     UserInputBufferIndex = 0;//points to address of next unfilled index

int     commandArgumentNumber = 0; //his is an int to allow commands to have multiple arguents

//This is the message sent if the command was not found. %s replaced by command received.
#define StringIfCommandNotRecognized "Command \"%s\" not recognized."
// length of the above string, but it removes the 2chars '%s', replacing with length of command
#define StringIfCommandNotRecognizedLength 31
//(strlen(StringIfCommandNotRecognized) -2 + COMMAND_LENGTH)



/*
 * Stores the char provided in the lastCharBeforeReturn variable.
 * If the character given is a newline character, it returns true and doesn't store it.
 */
bool storeInLastCharUntilEnter(char nextChar){
    if (nextChar == CommandEndChar){return true;}
    else{ lastCharBeforeEnter=nextChar; return false;}
}

/*
 * Clears the Last Character variable. Sets to NULL_CHAR
 */
void clearLastCharBeforeEnter(){
    lastCharBeforeEnter = NULL_CHAR;
}

/*
 * Stores char provided into the UserInputBuffer if there is room for it.
 * If character is an enter character, don't add and return true otherwise false.
 *
 * TODO this should properly deal with backspaces
 */
bool storeInUserInputBuffer(char nextChar){
    if (nextChar == CommandEndChar) {return true;} //inform a newline was received
    else if (UserInputBufferIndex > UserInputBufferLength) {return false;} //don't add if it would overflow buffer
    else {
        UserInputBuffer[UserInputBufferIndex] = nextChar;
        UserInputBufferIndex++;
        return false;
    }
}

/*
 * Clears UserInputBuffer by decrementing from index, replacing with null characters.
 */
void clearUserInputBuffer(){
    for(;UserInputBufferIndex>0;UserInputBufferIndex--){
        UserInputBuffer[UserInputBufferIndex-1] = NULL_CHAR; //clear the character moving to
    }
}

void displayLastCharPrompt(char *Prompt){
    sendStringAndNewLine(Prompt);
    char Buffer[12]; //13 is the length of the string below after sprintf evaluates it.
    sprintf(Buffer,"Selection: %c",lastCharBeforeEnter);
    sendStringAndNewLine(Buffer);
}

void displayUserInputBuffer(char *Prompt){
    sendStringAndNewLine(Prompt);
    sendStringAndNewLine(UserInputBuffer);
}

//========================================================= WATER PLANT CMD_WaterPlant ===================================================================

/* This command picks which plant to water and alters the timer to make that happen
 * WTR - water a plant, then restart delay
 *  | Which Plant? [0-4]
 */

void displayWaterPlant(void){
    displayLastCharPrompt("Which Plant To you want to water? [0-4]");
    displayMessage = false; //mark display as performed
}

void startWaterPlant(void){
    ActiveState = WaterPlant;
    displayMessage = true;
    clearLastCharBeforeEnter();
}

void addCharWaterPlant(char nextChar){
    //Store the next char, perform command if enter pressed
    if(storeInLastCharUntilEnter(nextChar)){
        //Perform command with information provided, pulling char from LastCharBeforeEnter
        completeWaterPlant(lastCharBeforeEnter);
    }
    //prompt to display whenever recieves new character.
    displayMessage = true;
}

//called once all requirements and need to perform tasks and return control
void completeWaterPlant(char timerSelection) {
    switch (timerSelection){
    case '0':
        startNewWaterCycle(&Timer0);
        break;
    case '1':
        startNewWaterCycle(&Timer1);
        break;
    case '2':
        startNewWaterCycle(&Timer2);
        break;
    case '3':
        startNewWaterCycle(&Timer3);
        break;
    case '4':
        startNewWaterCycle(&Timer4);
        break;
    default:
        break;
    }
    sendStringAndNewLine("Implemented: Perform Water Plant");

    //Hand control to command
    ActiveState = Command;
}
// ============================================================ END WATER PLANT =========================================================================

// ======================================================= SetTimerLength CMD_SetTimerLength ============================================================

/*
 * TME - Set the time of a timer.
 *  | Do you want to set (W)atering Time or (D)elay time?
 *  | Which Timer? [0-4]
 *  | How Long? (input a time string. e.g. "1d 5h 3m 2s 1ms")
 */

//character storing watering char
char wateringChar = 0;

void displaySetTimerLength(void){
    switch(commandArgumentNumber){
    case 0: //first argument, water or delay;
        displayLastCharPrompt("Do you want to set the (W)atering Time or (D)elay Time?");
        break;
    case 1: //second argument, plant select;
        displayLastCharPrompt("Which Plant To you want to set this time for? [0-4]");
        break;
    case 2: //third argument, length of delay;
        displayUserInputBuffer("Type the time you want it to take. i.e. '5d 2h 45m 30s 230ms'");
        break;
    }

    displayMessage = false; //mark display as performed
}

void startSetTimerLength(void){
    ActiveState = SetTimerLength;
    displayMessage = true;
    wateringChar = 0;
    commandArgumentNumber = 0;
    clearLastCharBeforeEnter();
    clearUserInputBuffer();
}

void addCharSetTimerLength(char nextChar){
    switch(commandArgumentNumber){
    case 0:
        if(storeInLastCharUntilEnter(nextChar)){
            //Save first value if enter pressed, advance to next argument
            wateringChar = lastCharBeforeEnter;
            commandArgumentNumber++;
            clearLastCharBeforeEnter();
        }
        break;
    case 1:
        if(storeInLastCharUntilEnter(nextChar)){
            //Save first value if enter pressed, advance to next argument
            commandArgumentNumber++;
        }
        break;
    case 2:
        if(storeInUserInputBuffer(nextChar)){
            //all arguments provided, give response
            completeSetTimerLength(wateringChar,lastCharBeforeEnter,UserInputBuffer);
        }
        break;
    }
    //prompt to display whenever recieves new character.
    displayMessage = true;
}
//TODO implement
//called once all requirements and need to perform tasks and return control
void completeSetTimerLength(char waterOrDelay,char timerSelection,char *TimeSetting) {
    sendStringAndNewLine("Implement TimerSelection. Time string is:");
    sendStringAndNewLine(TimeSetting);
    switch (timerSelection){
    case '0':
        if(waterOrDelay=='W' || waterOrDelay=='w'){
            stringToTime(TimeSetting,&(Timer0.TimerTimes.WateringLength));
        }else if(waterOrDelay=='D' || waterOrDelay=='d'){
            stringToTime(TimeSetting,&(Timer0.TimerTimes.WaitLength));
        }
        updateTimerTickSettings(&Timer0);
        break;
    case '1':
        if(waterOrDelay=='W' || waterOrDelay=='w'){
            stringToTime(TimeSetting,&(Timer1.TimerTimes.WateringLength));
        }else if(waterOrDelay=='D' || waterOrDelay=='d'){
            stringToTime(TimeSetting,&(Timer1.TimerTimes.WaitLength));
        }
        updateTimerTickSettings(&Timer1);
        break;
    case '2':
        if(waterOrDelay=='W' || waterOrDelay=='w'){
            stringToTime(TimeSetting,&(Timer2.TimerTimes.WateringLength));
        }else if(waterOrDelay=='D' || waterOrDelay=='d'){
            stringToTime(TimeSetting,&(Timer2.TimerTimes.WaitLength));
        }
        updateTimerTickSettings(&Timer2);
        break;
    case '3':
        if(waterOrDelay=='W' || waterOrDelay=='w'){
            stringToTime(TimeSetting,&(Timer3.TimerTimes.WateringLength));
        }else if(waterOrDelay=='D' || waterOrDelay=='d'){
            stringToTime(TimeSetting,&(Timer3.TimerTimes.WaitLength));
        }
        updateTimerTickSettings(&Timer3);
        break;
    case '4':
        if(waterOrDelay=='W' || waterOrDelay=='w'){
            stringToTime(TimeSetting,&(Timer4.TimerTimes.WateringLength));
        }else if(waterOrDelay=='D' || waterOrDelay=='d'){
            stringToTime(TimeSetting,&(Timer4.TimerTimes.WaitLength));
        }
        updateTimerTickSettings(&Timer4);
        break;
    }

    //Hand control to command
    ActiveState = Command;
}

//=========================================================== END SetTimerLength ========================================================================

// ======================================================== COMMAND BUFFER ================================================================================

char CommandBuffer[COMMAND_LENGTH + 1]; //holds 3 characters for commands (+1 for null)
unsigned short CommandBufferIndex = 0; // the index of the next character to add

void displayCommandBuffer(){
    char buffer[10+COMMAND_LENGTH+1]; // the space required to command length to the below string. (10 is length of string) (1 null char at end)
    sprintf(buffer,"Command: %s",CommandBuffer);
    sendStringAndNewLine(buffer);
    displayMessage = false;
}

void evaluateCommandBuffer(void){
    //If we are evaluating a full command
    if(CommandBufferIndex >= COMMAND_LENGTH){
        //Check if it is a valid command
        if (strcmp(CommandBuffer,CMD_WaterPlant)==0){ //implemented
            startWaterPlant();
            clearCommandBuffer();
            return;

        }/* else if(strcmp(CommandBuffer,CMD_StopTimer)==0){
            //ActiveState = StopTimer;
            pauseFunctions();
            clearCommandBuffer();
            return;

        }*/ else if(strcmp(CommandBuffer,CMD_SetTimerLength)==0){ //implemented
            startSetTimerLength();
            clearCommandBuffer();
            return;

        }/*else if(strcmp(CommandBuffer,CMD_DecideActionWhenEmpty)==0){
            ActiveState = DecideActionWhenEmpty;
            clearCommandBuffer();
            return;

        }*/ /*else if(strcmp(CommandBuffer,CMD_SaveTimerConfig)==0){
            ActiveState = SaveTimerConfig;
            clearCommandBuffer();
            return;

        }*/ /*else if(strcmp(CommandBuffer,CMD_ResetToTimerConfig)==0){
            ActiveState = ResetToTimerConfig;
            clearCommandBuffer();
            return;

        }*/ else if(strcmp(CommandBuffer,CMD_PrintAllToScreen)==0){ //implemented
            ActiveState = PrintAllToScreen;
            clearCommandBuffer();
            return;

        }
        //continue adding valid commands here.
    }

    //inform that no command was recognized
    //char* buffer[StringIfCommandNotRecognizedLength];
    //sendStringAndNewLine(sprintf(buffer,StringIfCommandNotRecognized,CommandBuffer));

    clearCommandBuffer(); //clear commands when finished
    return;
}

/**
 * Clear the command Buffer by fully populating with clear Chars, reset CommmandBufferIndex to 0
 */
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
            case CommandEndChar: //enter Pressed
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
        case CommandEndChar: // hit enter
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
        return false;
}


//===================================================== END COMMAND BUFFER ==============================================================================

// ========================================================== Primary Operations =========================================================================

/*
 * This initializes the communication module. Run this prior to using.
 */
void initCommunication(){
    clearCommandBuffer();//reset the command buffer
    ActiveState = Command;
    displayMessage = true;

#ifdef COMMUNICATION_USE_BLUETOOTH_UART
    configHFXT();
    ConfigureUART_A2();
#endif

#ifdef COMMUNICATION_USE_DIRECT_UART
    configHFXT();
    ConfigureUART_A0();
#endif

#ifdef COMMUNICATION_USE_CONSOLE
    //No initialization needed
#endif
}


/*
 * This is called to give communication the ability to send information
 */
void displayCommunication(void){
    switch(ActiveState){
    case Command:
        if (displayMessage) {displayCommandBuffer();} //display if need to display
        break;
    case WaterPlant:
        if (displayMessage) {displayWaterPlant();}
        break;
    case SetTimerLength:
        if (displayMessage) {displaySetTimerLength();}
        break;
    case PrintAllToScreen:
        //performPrintAllToScreen();
        printAllTimerSettings();
        printSystemSettings();
        ActiveState=Command;
        break;
    }
}

/*
 * This is called whenever information is recieved.
 */
void recieveCharForCommunication(char recieved){
    switch (ActiveState){
    case Command:
        displayMessage = addCharToCommandBuffer(recieved);//display new message if change
        break;
    case WaterPlant:
        addCharWaterPlant(recieved);
        break;
    case SetTimerLength:
        addCharSetTimerLength(recieved);
        break;

    //ignore any characters received here
    case PrintAllToScreen:
    default:
        break;
    }
}

// this is how we specify what string to send, don't include new line
void sendString(char *Buffer){

#ifdef COMMUNICATION_USE_BLUETOOTH_UART
    SendCharArray_A2(Buffer);
#endif

#ifdef COMMUNICATION_USE_DIRECT_UART
    SendCharArray_A0(Buffer);
#endif

#ifdef COMMUNICATION_USE_CONSOLE
    printf(Buffer);
#endif
}

// this is how we specify what string to send
//Includes a new line after message.
void sendStringAndNewLine(char *Buffer){

#ifdef COMMUNICATION_USE_BLUETOOTH_UART
    SendCharArray_A2(Buffer);
    SendCharArray_A2(NextLine);
#endif

#ifdef COMMUNICATION_USE_DIRECT_UART
    SendCharArray_A0(Buffer);
    SendCharArray_A0(NextLine);
#endif

#ifdef COMMUNICATION_USE_CONSOLE
    printf(Buffer);
    printf(NextLine);
#endif
}

/*
 * This is called to tell this module to read the next char for into communication.
 */
void readNextCharForCommunication(){
    char recievedChar;

#ifdef COMMUNICATION_USE_BLUETOOTH_UART
    recievedChar = GetChar_A2();
    if (recievedChar != NULL){
        recieveCharForCommunication(recievedChar);
    }
#endif

#ifdef COMMUNICATION_USE_DIRECT_UART
    recievedChar = GetChar_A0();
    if (recievedChar != NULL){
        recieveCharForCommunication(recievedChar);
    }
#endif

#ifdef COMMUNICATION_USE_CONSOLE
    // Do not have a way for Console to input characters
#endif
}
