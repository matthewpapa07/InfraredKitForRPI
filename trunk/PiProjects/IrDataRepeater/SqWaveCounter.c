#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define ITERATIONS 100
#define INPUT_PIN  3
#define OUTPUT_PIN 4

int main (int argc, char *argv[])
{
  int i, val = 1;
//  long int time1;
//  long int time2;
  struct timespec timeStruct;
  unsigned long int results[ITERATIONS];
//  int SleepValue;

  wiringPiSetupGpio(); // Broadcomm GPIO Pinout

  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);

  digitalWrite(OUTPUT_PIN, 0);
  val = digitalRead(INPUT_PIN);

  printf("Start Program\n");

  clock_gettime(CLOCK_REALTIME, &timeStruct);
//  time1 = timeStruct.tv_nsec;

  for(i=0; i<ITERATIONS; i++)
  {
    while((val == digitalRead(INPUT_PIN)))
    {
      
    }
    val = digitalRead(INPUT_PIN);
    clock_gettime(CLOCK_REALTIME, &timeStruct);
    results[i] = timeStruct.tv_nsec;
//    printf("m");
//    usleep(10);
    //delay(1);
  }
  printf("\n Middle \n");

  for(i=0; i<ITERATIONS; i++)
  {
    printf("Number %d is %lu\n", i, results[i]);
  }

  for(i=0; i< ITERATIONS - 1; i++)
  {
   results[i] = results[i+1] - results[i];
  }

  printf("Waiting\n");
  delay(3000);
  printf("Retransmitting\n");
  val = 0;
  for(i=0; i<ITERATIONS - 1; i++)
  {
    //SleepValue = (results[i+1] - results[i]);///1000 ;
//    printf("Sleeping %d and outputting %d \n", SleepValue, val);
    //usleep(SleepValue);
    digitalWrite(OUTPUT_PIN, val);
//    timeStruct.tv_nsec = results[i];// SleepValue;
    timeStruct.tv_nsec = 9000000; 
    timeStruct.tv_sec = 0;
    nanosleep(&timeStruct, NULL);
    if(val >0)
      val = 0;
    else
      val = 1;
  }

  printf("End Program\n");
  return 0;
}

int SquareWave(void)
{
  int i;
  wiringPiSetupGpio(); // Broadcomm GPIO Pinout

  pinMode(3, INPUT);
  pinMode(4, OUTPUT);

  printf("Start Program\n");
  for(i=0; i < 3000; i++)
  {
    digitalWrite(4, 0);
    usleep(1000);
    digitalWrite(4, 1);
    usleep(1000);
  }
  printf("End Program\n");

  return 0;
}
