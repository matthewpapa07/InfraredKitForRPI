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
#define BAUD_RATE_NSECS   434336
//#define BAUD_RATE_NSECS   433656
#define ONE_BIL_NSECS     1000000000

unsigned long int results[ITERATIONS];
struct timespec WatchdogTimer;

void ChangePwm(int HighOrLo);
void WatchDogSet(void);
void PrintResultsArray(int NumResults);
int IsWatchDogExpired(void);
int RecordData(void);
void Transmit(void);
int IsCurrentGreaterThanFuture(struct timespec *currentTime, struct timespec *futureTime);

//
// Initialize GPIOs
//
void InitializeGpios(void)
{
  wiringPiSetupGpio(); // Broadcomm GPIO Pinout

  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(PWM_PIN, PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);
  pwmSetClock(50); //Clock divided by this value is how fast the range counter increments
  pwmSetRange(10); // Half of the counter low half high

  ChangePwm(0);

  digitalWrite(OUTPUT_PIN, 0);
}

//
// Main loop
//
int main (int argc, char *argv[])
{
  int i ;
//  int NumberOfValues;
  InitializeGpios();

  // Initialize Array
  for(i = 0; i < ITERATIONS; i++)
    results[0] = 0;

  printf("Setting Priority %d\n", piHiPri(99));

  Transmit();
//  NumberOfValues = RecordData();
/*
  for(i=0; i< 1000; i++)
  {
    ChangePwm(1);
    delay(13);
    ChangePwm(0);
    delay(13);
  }
*/
  return 0;
}

//
// Waits on an input to begin recording infrared input
// Returns number of entries in iterations
//
int RecordData(void)
{
  int i, val;
  struct timespec timeStruct;

  ChangePwm(0); // Null this out to avoid any interference
  WatchDogSet();

  printf("Waiting To Start Recording\n");
  // initial value
  val = digitalRead(INPUT_PIN);

  if(val != 1)
    printf("Signal in process, killing RecordData\n");

  // initial time (t0)
  clock_gettime(CLOCK_REALTIME, &timeStruct);

  //
  // When waveform is on signal is low, otherwise it is high. Unless it is on for too long, the recevier shows low again
  //
  for(i=0; i<ITERATIONS && !IsWatchDogExpired(); i++)
  {
    while(val == digitalRead(INPUT_PIN) && !IsWatchDogExpired())
    {
    }
    clock_gettime(CLOCK_REALTIME, &timeStruct);
    val = digitalRead(INPUT_PIN);

    results[i] = timeStruct.tv_nsec;
  }

  if(i == (ITERATIONS - 1))
    printf("WARNING: Message buffer overflow. Increase buffer size.\n");

  return i;
}

void WatchDogSet(void)
{
  clock_gettime(CLOCK_REALTIME, &WatchdogTimer);

  WatchdogTimer.tv_sec += WATCHDOG_SECONDS;

//  printf("Watchdog Timer will expire in %d Sec\n", (int)WatchdogTimer.tv_sec);

}

int IsWatchDogExpired(void)
{
  struct timespec CompareTime;
  clock_gettime(CLOCK_REALTIME, &CompareTime);

  if(CompareTime.tv_sec > WatchdogTimer.tv_sec)
    return 1;
   else
    return 0;
}

//
// Transmit Array
//
void PrintResultsArrayRaw(int NumResults)
{
  int i, val = 1;
  unsigned long int NetTime;
  printf("\n Printing Results \n");

  for(i = 1; i < (NumResults - 1); i++)
  {
    NetTime = results[i] - results[i - 1];
//    printf("Table # %d Logic Value %d for %lu\n", i, val, NetTime);
    printf("%lu\n", NetTime);
    if(val == 0)
      val = 1;
    else
      val = 0;
  }

//  for(i=0; i< ITERATIONS - 1; i++)
//  {
//   results[i] = results[i+1] - results[i];
//  }
}

//
// Get minimum value to attempt to discover a minimum bit resolution
//
int GetSingleBitTime(int NumResults)
{
  int i;
  unsigned long int min = 0;

  // Arbitrarily large compare number
  min = 0xFFFFFFFF;

  for(i = 1; i < (NumResults - 1); i++)
  {
    if(min > (results[i] - results[i - 1]))
      min = results[i] - results[i - 1];
  }
  return (int) min;
}

void Transmit(void)
{
  int i;

  struct timespec currentTime;
  struct timespec futureTime;

  ChangePwm(0); // Null this out to avoid any interference

  printf("Transmitting\n");
  clock_gettime(CLOCK_REALTIME, &futureTime);

 // Cant use a simple delay between transmissions here because the delays aer not precise (linux is not not quite an RTOS)
 // The clock however should always be 100% accurate, so we will poll that and change output accordingly. Ideally this will
 // be precise enough
  for(i = 0; i < OUTBITS_LENGTH; i++)
  {
    ChangePwm(OutBits[i]);
    futureTime.tv_nsec = (futureTime.tv_nsec + BAUD_RATE_NSECS)%ONE_BIL_NSECS; // Increment and account for any overflow with modulus
    futureTime.tv_sec = futureTime.tv_sec + (int)((futureTime.tv_nsec + BAUD_RATE_NSECS)/ONE_BIL_NSECS);
    do
    {
      clock_gettime(CLOCK_REALTIME, &currentTime);
    }
    while(!IsCurrentGreaterThanFuture(&currentTime, &futureTime));
  }

  printf("End Transmit\n");
}

int IsCurrentGreaterThanFuture(struct timespec *currentTime, struct timespec *futureTime)
{
  if(currentTime->tv_sec > futureTime->tv_sec)
    return 1;
  if(currentTime->tv_sec == futureTime->tv_sec)
  {
    if(currentTime->tv_nsec > futureTime->tv_nsec)
      return 1;
  }
  // Return No
  return 0;
}


void ChangePwm(int HighOrLo)
{
  // pwmFrequency in Hz = 19.2e6 Hz / pwmClock (freq divisor) / pwmRange (counter that increments).
  // SetRange is 2 to 4095
  //
  //pwmWrite(1, 75);  //theoretically 50 (1ms) to 100 (2ms) on my servo 30-130 works okpinMode(1, PWM_OUTPUT);

  if(HighOrLo == 0)
    pwmWrite(PWM_PIN, 0);  //theoretically 50 (1ms) to 100 (2ms) on my servo 30-130 works okpinMode(1, PWM_OUTPUT);
  else
    pwmWrite(PWM_PIN, 5);
}

int ReadSensor(void)
{
	// BUFFER TO RECEIVE
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;
        int i;

	// EMPTY BUFFER
	for (i=0; i< 5; i++) bits[i] = 0;

	// REQUEST SAMPLE
	pinMode(SENSOR_PIN, OUTPUT);
	digitalWrite(SENSOR_PIN, LOW);
	delay(18);
	digitalWrite(SENSOR_PIN, HIGH);
	delayMicroseconds(40);
	pinMode(SENSOR_PIN, INPUT);

	// ACKNOWLEDGE or TIMEOUT
	unsigned int loopCnt = 10000;
	while(digitalRead(SENSOR_PIN) == LOW)
		if (loopCnt-- == 0) return -1;

	loopCnt = 10000;
	while(digitalRead(SENSOR_PIN) == HIGH)
		if (loopCnt-- == 0) return -1;

	// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
	for (i=0; i<40; i++)
	{
		loopCnt = 10000;
		while(digitalRead(SENSOR_PIN) == LOW)
			if (loopCnt-- == 0) return -1;

		unsigned long t = micros();

		loopCnt = 10000;
		while(digitalRead(SENSOR_PIN) == HIGH)
			if (loopCnt-- == 0) return -1;

		if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
		if (cnt == 0)   // next byte?
		{
			cnt = 7;    // restart at MSB
			idx++;      // next byte!
		}
		else cnt--;
	}

	// WRITE TO RIGHT VARS
        // as bits[1] and bits[3] are allways zero they are omitted in formulas.
	humidity    = bits[0];
	temperature = bits[2];

	uint8_t sum = bits[0] + bits[2];

	if (bits[4] != sum) return DHTLIB_ERROR_CHECKSUM;
	return 0;
}
