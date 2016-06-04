
#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define WATCHDOG_SECONDS  10
#define ITERATIONS        900
#define INPUT_PIN         3
#define OUTPUT_PIN        4
#define SENSOR_PIN        23
#define PWM_PIN           18
//#define BAUD_RATE_NSECS   434336
#define BAUD_RATE_NSECS   433656
#define ONE_BIL_NSECS     1000000000

// Temperature/Humidity defines. All temperatures are in F
#define DEFAULT_SET_TEMP    72
#define TOO_HOT_TEMP        76
#define MAX_HUMIDITY        55
#define SET_RETRIES         5

// Infrared Functions
void InitializeGpios(void);
void ChangePwm(int HighOrLo);
void WatchDogSet(void);
void PrintResultsArray(int NumResults);
int  IsWatchDogExpired(void);
int  RecordData(void);
void TransmitPattern(char OutBits[], int OutBitsSize)
int  IsCurrentGreaterThanFuture(struct timespec *currentTime, struct timespec *futureTime);
void TempHumdityControlLoop(void);


// Sensor Functions
int    ReadSensor(int *temperature, int *humidity);
double dewPoint(double celsius, double humidity);
double Kelvin(double celsius);
double Fahrenheit(double celsius);
