/*
 * Switches.c
 *
 *  Created on: Feb 16, 2024
 *      Author: zawaidhm
 */

#include "Switches.h"
#include <stdbool.h> //true, false, bool
#include "Pump.h"


//S2 digital input w/ internal pull-up resistor
//ren =1, out =1
//dir =0, sel0=0, sel1=0
void configureAllPlantSwitches(void){
    switchPort ->SEL1&=~(sw0Mask|sw1Mask|sw2Mask|sw3Mask|sw4Mask|levelMask|masterMask);
    switchPort ->SEL0&=~(sw0Mask|sw1Mask|sw2Mask|sw3Mask|sw4Mask|levelMask|masterMask);
    switchPort ->DIR&=~(sw0Mask|sw1Mask|sw2Mask|sw3Mask|sw4Mask|levelMask|masterMask);
    switchPort ->REN|=(sw0Mask|sw1Mask|sw2Mask|sw3Mask|sw4Mask|levelMask|masterMask);
    switchPort ->OUT|=(sw0Mask|sw1Mask|sw2Mask|sw3Mask|sw4Mask|levelMask|masterMask);
}
SwitchStatuses SwitchStates={
         ._0isPressed=false,
         ._1isPressed=false,
         ._2isPressed=false,
         ._3isPressed=false,
         ._4isPressed=false,
         .levelIsLow=false,
         .masterIsPressed=false,
};

//debounce for no less than 5ms
void debounce(void){
//delay time for debouncing switches
    int currCycle=DEBOUNCE_COUNT;
    while(currCycle>0){
        currCycle--;
    }//end while
} //end debounce()

void checkAllSwitches(void){
    char switchValue;
    switchValue =switchPort -> IN & sw0Mask;
    if(switchValue==0){ //pressed
        debounce();
        SwitchStates._0isPressed=1;
    }else{
        SwitchStates._0isPressed=0;

    }
    switchValue= switchPort -> IN & sw1Mask;
    if(switchValue==0){ //pressed
        debounce();
        SwitchStates._1isPressed=1;

    }else{
        SwitchStates._1isPressed=0;

    }
    switchValue= switchPort -> IN & sw2Mask;
    if(switchValue==0){ //pressed
        debounce();
        SwitchStates._2isPressed=1;
    }else{
        SwitchStates._2isPressed=0;
    }
    switchValue= switchPort -> IN & sw3Mask;
    if(switchValue==0){ //pressed
        debounce();
        SwitchStates._3isPressed=1;
    }else{
        SwitchStates._3isPressed=0;
    }
    switchValue= switchPort -> IN & sw4Mask;
    if(switchValue==0){ //pressed
        debounce();
        SwitchStates._4isPressed=1;
    }else{
        SwitchStates._4isPressed=0;
    }
    switchValue= switchPort -> IN & levelMask;
    if(switchValue==0){ //pressed
        debounce();
        SwitchStates.levelIsLow=1;
    }else{
        SwitchStates.levelIsLow=0;
    }
    switchValue= switchPort -> IN & masterMask;
    if(switchValue==0){ //pressed
        debounce();
        SwitchStates.masterIsPressed=1;
    }else{
        SwitchStates.masterIsPressed=0;
    }
}

bool checkSW0(void){
   if(!(switchPort -> IN & sw0Mask)){ //pressed
       debounce();
       SwitchStates._0isPressed=1;
   }else{
       SwitchStates._0isPressed=0;
   }
   return SwitchStates._0isPressed;
}
bool checkSW1(void){
   if(!(switchPort -> IN & sw1Mask)){ //pressed
       debounce();
       SwitchStates._1isPressed=1;
   }else{
       SwitchStates._1isPressed=0;
   }
   return SwitchStates._1isPressed;
}
bool checkSW2(void){
   if(!(switchPort -> IN & sw2Mask)){ //pressed
       debounce();
       SwitchStates._2isPressed=1;
   }else{
       SwitchStates._2isPressed=0;
   }
   return SwitchStates._2isPressed;

}
bool checkSW3(void){
   if(!(switchPort -> IN & sw3Mask)){ //pressed
       debounce();
       SwitchStates._3isPressed=1;
   }else{
       SwitchStates._3isPressed=0;
   }
   return SwitchStates._3isPressed;
}
bool checkSW4(void){
   if(!(switchPort -> IN & sw4Mask)){ //pressed
       debounce();
       SwitchStates._4isPressed=1;
   }else{
       SwitchStates._4isPressed=0;
   }
   return SwitchStates._4isPressed;
}
bool checkLevelSW(void){
   if(!(switchPort -> IN & levelMask)){ //pressed
       debounce();
       SwitchStates.levelIsLow=1;
   }else{
       SwitchStates.levelIsLow=0;
   }
   return SwitchStates.levelIsLow;
}
bool checkMasterSW(void){
   if(!(switchPort -> IN & masterMask)){ //pressed
       debounce();
       SwitchStates.masterIsPressed=1;
   }else{
       SwitchStates.masterIsPressed=0;
   }
   return SwitchStates.masterIsPressed;
}


