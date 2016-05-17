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
#define PWM_PIN           18

unsigned long int results[ITERATIONS];
struct timespec WatchdogTimer;

void ChangePwm(int HighOrLo);
void WatchDogSet(void);
void PrintResultsArray(int NumResults);
int IsWatchDogExpired(void);
int RecordData(void);

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
  int NumberOfValues;
  InitializeGpios();

  for(i = 0; i < ITERATIONS; i++)
    results[0] = 0;

  printf("Setting Priority %d\n", piHiPri(99));

  NumberOfValues = RecordData();
  PrintResultsArray(NumberOfValues);
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
  delay(1);
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
void PrintResultsArray(int NumResults)
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

void Transmit(int NumResults)
{
  int val;
  int i;

  val = 0;
  printf("Retransmitting\n");

  delay(3000);
  for(i=0; i<ITERATIONS - 1; i++)
  {
    if(val >0)
      val = 0;
    else
      val = 1;
  }

  printf("End Retransmit\n");
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

