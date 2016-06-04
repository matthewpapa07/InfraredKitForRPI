#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define ITERATIONS 770
#define INPUT_PIN  3
#define OUTPUT_PIN 4

int main (int argc, char *argv[])
{
  int i, j, val;
//  long int time1;
//  long int time2;
  struct timespec timeStruct;
  struct timespec testTime;
  unsigned long int results[ITERATIONS];
//  int SleepValue;
  bool ExitLoop = false;

  wiringPiSetupGpio(); // Broadcomm GPIO Pinout

  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);

  digitalWrite(OUTPUT_PIN, 0);
  val = digitalRead(INPUT_PIN); // Input is pulled down by default

  printf("First Value %d read.\n",val);

//  printf("Start Recording  Time\n");

  while(val == digitalRead(INPUT_PIN)){}//{printf(".");}

//  printf("a\n");
  clock_gettime(CLOCK_REALTIME, &timeStruct);
  results[0] = timeStruct.tv_nsec;
//  time1 = timeStruct.tv_nsec;

  for(i=1, j=0; (i < ITERATIONS) && (ExitLoop == false); i++)
  {
//    printf("b\n");
    while(val == digitalRead(INPUT_PIN))
    {
      j++;
//      if(j > 50000) // Count cycles thru the loop to save time if ther is a transition
      {
//        printf("c\n");
        clock_gettime(CLOCK_REALTIME, &testTime);
        if((testTime.tv_nsec - results[i]) > 2066516115)
        {
//          printf("d\n");
          printf("Results i %lu\n", results[i]);
          printf("Results Struct %lu\n", testTime.tv_nsec);
          ExitLoop = true;
	  break;
        }
      }
    }
//    printf("e\n");
    val = digitalRead(INPUT_PIN);
    clock_gettime(CLOCK_REALTIME, &timeStruct);
    results[i] = timeStruct.tv_nsec;
    j = 0;
//    printf("m");
//    usleep(10);
    //delay(1);
  }
  printf("\n Data Gathering Complete. I is %d \n", i);

  for(j=0; j < i; j++)
  {
    //printf("%lu\n", results[j]);
  }


/*
  printf("Waiting\n");
  delay(5000);
  printf("Retransmitting\n");
  val = 0;
  for(i=1; i<ITERATIONS - 1; i++)
  {
    digitalWrite(OUTPUT_PIN, val);
    if(val >0)
      val = 0;
    else
      val = 1;
    SleepValue = (results[i] - results[i-1])/1000 ;
    printf("Sleeping %d microseconds\n", SleepValue);
    usleep(SleepValue);
  }

*/
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
