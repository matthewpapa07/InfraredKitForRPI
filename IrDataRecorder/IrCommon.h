#ifndef IRCOMMON_H
#define IRCOMMON_H

#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "IrHelpers.h"

// Local includes


#define WATCHDOG_SECONDS  10
#define ITERATIONS        900
#define INPUT_PIN         3
#define OUTPUT_PIN        4
#define PWM_PIN           18
//#define BAUD_RATE_NSECS   434336
#define BAUD_RATE_NSECS   433656
#define ONE_BIL_NSECS     1000000000
#define STARTING_INPUT    1
//#define T_DEBUG 1

void SendMessage (void);
void SignalToHex(void);

#endif /*IRCOMMON_H*/
