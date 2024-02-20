/*
 * Speaker.c
 *
 *  Created on: Feb 19, 2024
 *      Author: zawaidhm
 */

#include "Speaker.h"

//===================================
//Configure Speaker as Timer A0 CCR1 output pin
//Configure Timer A0 to produce sound on CCR1 output compare

void PlayerConfiguraion(void) {

    SpeakerPort->DIR |= Speaker;            // set P2.4 as output
    SpeakerPort->SEL0 |= Speaker;           // P2.4 set to TA0.1
    SpeakerPort->SEL1 &= ~Speaker;  // Option 0b01
//set Timer A0 CCR0 compare for period and CCR1 compare for duty cycle of a note
//configure Timer_A0
//     TIMER_A0->CCTL[1] = 0b01100000;
//     TIMER_A0->CTL = TIMER_A0->CTL | 0b010000 ; //set up mode
//     TIMER_A0->CTL = TIMER_A0->CTL | 0b1000000000 ; //set source clock as smclk
//     TIMER_A0->CTL = TIMER_A0->CTL | 0b100 ; //reset TA0R
//     TIMER_A0->CTL = TIMER_A0->CTL | 0b10000000 ; //prescaler 4
      TIMER_A0->CTL = 0x294;
      TIMER_A0->CCTL[1] = 0x060;

}

//================================
//Play a note by sending its Timer A0 cycle count to this subroutine
//This could be insert in the interrupt service
void PlayNote (unsigned int CurrentNote) {
    TIMER_A0->CCR[0] = CurrentNote - 1;
     // Set high pulse-width in CCR1 register (determines duty cycle)
     TIMER_A0->CCR[1] = (CurrentNote / 2) - 1;
}

//================================
//Play the note list once to test the notes
const uint16_t NotesSequence[] = {NOTEF4s, NOTEG5, RestNote, NOTEF4s, NOTEA4, NOTEB4, NOTEC5s, NOTED5, NOTEE5, NOTEF5s, NOTEG5, NULL};
void  PlayAllNotesOnce(const uint16_t notelist[]){
    static char noteindex;
    unsigned int delay;
    noteindex=0;
    while(notelist[noteindex]!=NULL) {
        PlayNote(notelist[noteindex]);
    noteindex=noteindex+1;
    for(delay = NoteDuration; delay != 0; delay--) {
     }
    } //end while()
}//end PlayAllNotesOnce(void)
