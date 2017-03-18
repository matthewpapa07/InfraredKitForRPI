#include "IrCommon.h"

struct timespec WatchdogTimer;



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


int BenchmarkPi(unsigned long int results[])
{

  int i;
  struct timespec timeStruct;

  WatchDogSet();

  printf("Waiting To Start Recording\n");


  // initial time (t0)
  clock_gettime(CLOCK_REALTIME, &timeStruct);

  //
  // When waveform is on signal is low, otherwise it is high. Unless it is on for too long, the recevier shows low again
  //
  for(i=0; i<ITERATIONS && !IsWatchDogExpired(); i++)
  {
    clock_gettime(CLOCK_REALTIME, &timeStruct);

    results[i] = timeStruct.tv_nsec;
  }

  if(i == (ITERATIONS - 1))
    printf("WARNING: Message buffer overflow. Increase buffer size.\n");

  printf("Recorded %d waves\n", i);

  return i;

}

//
// Waits on an input to begin recording infrared input
// Returns number of entries in iterations
//
int RecordData(unsigned long int results[])
{
  int i, val;
  struct timespec timeStruct;

  WatchDogSet();

  printf("Waiting To Start Recording\n");
  // initial value
  val = digitalRead(INPUT_PIN);

  if(val != STARTING_INPUT)
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

// Discard last value
  results[i] = 0;
  i--;

  printf("Recorded %d waves\n", i);

  return i;
}


unsigned long int DecodeSignal(unsigned long int results[], int ArrayLength)
{
  unsigned long int SortedResults[ITERATIONS];
  float remainder;
  unsigned int Stack[ITERATIONS*10];
  int i, p, q, sp;
//  unsigned long int BaudRateSamples[ITERATIONS];
  unsigned long int ProtoBaudRate;
  unsigned long int BaudRate;
  char HexValue[1024];

  BaudRate = 0;

  printf("DecodeSignal: FlattenTimeArray\n");
  // Flatten array
  FlattenTimeArray(results, ArrayLength);

  ///
  // Clear sorted array. Copy raw data array to sorted array
  //
  for(i=0; i< ITERATIONS; i++)
  {
    SortedResults[i] = results[i];
  }
  for(i=0; i< ITERATIONS*10; i++)
  {
    Stack[i] = 0;
  }
  for(i=0; i< 1024; i++)
  {
    HexValue[i] = 0;
  }

   printf("DecodeSignal: Quick sort.\n");
  // Sort the array small to large
   quickSort(SortedResults, 0, ArrayLength - 1);

   printf("DecodeSignal: Sort Done.\n");
  // Assume the smallest values are the base transfer rate
  // Pick a value that should be a part of this group
  i = 0;
  while((SortedResults[i] == 0)&& i < ITERATIONS)
  {
    i++;
  }
  ProtoBaudRate = SortedResults[i];
  p = i;
  printf("DecodeSignal: Proto baud rate %lu.\n", ProtoBaudRate);

  // Now get an average baud rate based off of all similar (within a given %) other values
  for(;(i<ArrayLength - 1) && ValueIsWithinPercent(SortedResults[i], ProtoBaudRate, 18); i++)
  {
    BaudRate += SortedResults[i];
  }

  printf("DecodeSignal: Instances of first baud %d.\n", i);
  if(i == 0)
  {
    printf("Only one sample of first baud! Warning\n");
  }
  else
  {
    BaudRate = BaudRate/(i-p);
    printf("Calculated Baud Rate from sorted array %lu\n", BaudRate);
  }
  #ifdef T_DEBUG
  printf("Calculated Baud Rate from sorted array %lu\n", BaudRate);

  PrintArray(results, SortedResults, ArrayLength);
  #endif

  q = STARTING_INPUT;
  sp = 0;
  printf("Deciphering bits from times\n");
  // Push binary values into a stack like structure
  for(i = 1; i < ArrayLength ;i++, p=0)
  {
    remainder = (float)results[i]/(float)BaudRate;
    p = (int)remainder; // Truncate number for values
    remainder = remainder - (float)p;
    if(remainder > .5) // Take remainder and see if it is big enough to constitute another value
      p++;
    #ifdef T_DEBUG
    printf("Q = %d : p is %d, i is %d, remainder is %f \n",q,p,i,remainder);
    #endif
    while(p > 0)
    {
      Stack[sp] = q; // set stack value
      p--;   //Decrement instances of q
      sp++;  //increment stack pointer
    }
    // Flip the bit for next group of values
    if(q == 1)
      q = 0;
    else
      q = 1;
  }

  #ifdef T_DEBUG
  PrintBitArray(Stack, sp);
  #endif

  TranslateBitsToString(HexValue, Stack, sp);

  printf("Output string %s \n", HexValue);
  return BaudRate;
}


void SignalToHex(void)
{
  int i;
  unsigned long int gResults[ITERATIONS];

//  wiringPiSetupGpio(); // Broadcomm GPIO Pinout
  wiringPiSetup(); // Wiring pi simplified GPIO pinout
  pinMode(INPUT_PIN, INPUT);
  // Initialize Array
  for(i = 0; i < ITERATIONS; i++)
   gResults[0] = 0;

  printf("Setting Priority %d\n", piHiPri(99));

  i = RecordData(gResults);

//  i = BenchmarkPi(gResults);
//  PrintArray(gResults, NULL, i);

  DecodeSignal(gResults, i);

}

