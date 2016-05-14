#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define ITERATIONS 100
#define INPUT_PIN  3
#define OUTPUT_PIN 4
#define PWM_PIN    18

unsigned long int results[ITERATIONS];

void ChangePwm(int HighOrLo);

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

  digitalWrite(OUTPUT_PIN, 0);
}

//
// Main loop
//
int main (int argc, char *argv[])
{
  int i ;
  InitializeGpios();

  for(i=0; i< 1000; i++)
  {
    ChangePwm(0);
    delay(10);
    ChangePwm(1);
    delay(10);
  }

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

  digitalWrite(OUTPUT_PIN, 0);
  val = digitalRead(INPUT_PIN);

  printf("Start Program\n");

  clock_gettime(CLOCK_REALTIME, &timeStruct);

  for(i=0; i<ITERATIONS; i++)
  {
    while((val == digitalRead(INPUT_PIN)))
    {
    }
    val = digitalRead(INPUT_PIN);
    clock_gettime(CLOCK_REALTIME, &timeStruct);
    results[i] = timeStruct.tv_nsec;
  }
  return i;
}

//
// Transmit Array
//
void PrintResultsArray(int NumResults)
{
  int i;

  printf("\n Middle \n");

  for(i=0; i<ITERATIONS; i++)
  {
    printf("Number %d is %lu\n", i, results[i]);
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

  pwmSetClock(50); //Clock divided by this value is how fast the range counter increments
  pwmSetRange(10); // Half of the counter low half high
  if(HighOrLo == 0)
    pwmWrite(PWM_PIN, 1024);  //theoretically 50 (1ms) to 100 (2ms) on my servo 30-130 works okpinMode(1, PWM_OUTPUT);
  else
    pwmWrite(PWM_PIN, 5);
}

