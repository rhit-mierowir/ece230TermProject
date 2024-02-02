/*! \file */
/**********************************************************
 * ECE230 Winter 2023-2024
 * MSP432p4111 Term Project
 * ECE230-02, Jianjian Song
 * February, 2 2023
 * Description: The file is used to test out in a bit of a messy way that the basic hardware for the project works
 *
 * Watering System Active: Blue_RGB_LED on
 * Water level low: LED1 on
 *
 * Author: Isaac Mierow
 *
 *                MSP432P4111
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> LED1
 *            |                  |
 *            |            P2.0  |---> LED2 red
 *            |            P2.1  |---> LED2 green
 *            |            P2.2  |---> LED2 blue
 *            |                  |
 *            |                  *****************************************************************/


#include "msp.h"

// LED Constants
#define LED_PORT P1
#define LED_MASK 0b1
#define RGB_LED_PORT P2
#define R_LED_MASK 0b1
#define G_LED_MASK 0b10
#define B_LED_MASK 0b100
#define RGB_LED_MASK (R_LED_MASK|G_LED_MASK|B_LED_MASK)

//Button Constants
#define WATER_BUTTON_PORT P1
#define WATER_BUTTON_MASK 0b10

//Water Level Sensor Constants
#define WATER_SENSOR_PORT P1
#define WATER_SENSOR_MASK 0b10000

boolean watering = 0; //is the watering system currently on

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	configureLEDs();
	configureButtons();

	while(1){
	    detectWateringButtonPress();
	    readWaterLevelSensor();

	}

}

void configureLEDs(void)
{
    //Set output, DIR=1
    LED_PORT->DIR |= LED_MASK;
    RGB_LED_PORT->DIR |= RGB_LED_MASK;

    //Set to default GPIO Pins SEL=0,0
    LED_PORT->SEL0 &= ~LED_MASK;
    RGB_LED_PORT->SEL0 &= ~RGB_LED_MASK;
    LED_PORT->SEL1 &= ~LED_MASK;
    RGB_LED_PORT->SEL1 &= ~RGB_LED_MASK;

    //Start with lights off OUT=0
    LED_PORT->OUT &= ~LED_MASK;
    RGB_LED_PORT->OUT &= ~RGB_LED_MASK;
}

void configureButtons(void)
{
    //Set input, DIR=0
    WATER_BUTTON_PORT->DIR &= ~WATER_BUTTON_MASK;

    //Set to default GPIO SEL=0,0
    WATER_BUTTON_PORT->SEL0 &= ~WATER_BUTTON_MASK;
    WATER_BUTTON_PORT->SEL1 &= ~WATER_BUTTON_MASK;

    //Set to enable pullup resistor REN=1
    WATER_BUTTON_PORT->REN |= WATER_BUTTON_MASK;

    //Set to pullup resistor OUT=1
    WATER_BUTTON_PORT->OUT |= WATER_BUTTON_MASK;
}

/* Watering System:
 * This system activates and deactivates the watering system by toggling every press of the Watering Button.
 * This also toggles the B_RGB_LED to on if the system is active, off otherwise.
 */

void detectWateringButtonPress(){
    if(WATER_BUTTON_PORT->IN & WATER_BUTTON_MASK) // if button is High, so not pressed
        return;
    //toggle the watering state;
    if (watering)
    {
        stopWatering();
    }else{
        startWatering();
    }
    debounce();
    while(WATER_BUTTON_PORT->IN & WATER_BUTTON_MASK); // wait until button no longer pressed
    debounce();
}

// Set Debounce values for script.
#define DEBOUNCE_PERIOD 5 //milliseconds
#define DEBOUNCE_LOOP_CYCLES 8 //cycles
#define DEBOUNCE_LOOP_COUNT DEBOUNCE_PERIOD*SYSTEM_CLOCK_FREQUENCY/DEBOUNCE_LOOP_CYCLES
void debounce(void){
    int i;
    for (i=DEBOUNCE_LOOP_COUNT;i!=0; i--); // each incrament is 8 cycles
//deley time for debouncing switches
} //end debounce()

void startWatering(void)
{
    //Set B_LED to on
    RGB_LED_PORT->OUT |= B_LED_MASK;

    //Report Turned on Watering
    watering = 1;

    // turn on watering system
}

void stopWatering(void)
{
    //Set LED1 to off
    RGB_LED_PORT->OUT &= B_LED_MASK;

    //Report Turned off watering
    watering=0;

    // turn off watering system
}

void configureLevelSensor()
{
    //Set input, DIR=0
    WATER_SENSOR_PORT->DIR &= ~WATER_SENSOR_MASK;

    //Set to default GPIO SEL=0,0
    WATER_SENSOR_PORT->SEL0 &= ~WATER_SENSOR_MASK;
    WATER_SENSOR_PORT->SEL1 &= ~WATER_SENSOR_MASK;

    //Set to enable pullup resistor REN=1
    WATER_SENSOR_PORT->REN |= WATER_SENSOR_MASK;

    //Set to pullup resistor OUT=1
    WATER_SENSOR_PORT->OUT |= WATER_SENSOR_MASK;
}

void readWaterLevelSensor()
{
    //Read the Sensor and condition on it to determine if the
}

void waterLevelLow()
{
    //Turn on LED1
    LED_PORT->OUT |= LED_MASK;

    //Other things to do when the water level is low
}
void waterLevelGood()
{
    //Turn off LED1
    LED_PORT->OUT &= ~LED_MASK;

    //Other things to do when the water level is good
}
