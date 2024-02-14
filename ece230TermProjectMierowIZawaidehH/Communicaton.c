#include "Communication.h"

#include "time.h"
#include "uart_routines2024.h"
#include "WateringTimer.h"


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
 */
