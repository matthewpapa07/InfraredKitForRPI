#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "OutBits.h"

#define WATCHDOG_SECONDS  10
#define ITERATIONS        900
#define INPUT_PIN         3
#define OUTPUT_PIN        4
#define SENSOR_PIN        5
#define PWM_PIN           18
//#define BAUD_RATE_NSECS   434336
#define BAUD_RATE_NSECS   433656
#define ONE_BIL_NSECS     1000000000

// Infrared Functions
void InitializeGpios(void);
void ChangePwm(int HighOrLo);
void WatchDogSet(void);
void PrintResultsArray(int NumResults);
int IsWatchDogExpired(void);
int RecordData(void);
void Transmit(void);
int IsCurrentGreaterThanFuture(struct timespec *currentTime, struct timespec *futureTime);


// Sensor Functions
int ReadSensor(int *temperature, int *humidity);
double dewPoint(double celsius, double humidity);
double Kelvin(double celsius);

