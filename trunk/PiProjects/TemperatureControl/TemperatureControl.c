#include "TemperatureControl.h"

// Bit patterns for dehumifify
#include "SignalOff.h"
#include "SignalOnDehumidify.h"

unsigned long int results[ITERATIONS];
struct timespec WatchdogTimer;

//
// Initialize GPIOs
//
void InitializeGpios(void)
{
  wiringPiSetupGpio(); // Broadcomm GPIO Pinout

  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(PWM_PIN, PWM_OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
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

  printf("Setting Priority to: %d . 0 Is the highest.\n", piHiPri(99));

  TempHumdityControlLoop();

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

void TransmitPattern(char OutBits[], int OutBitsSize)
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
  for(i = 0; i < OutBitsSize; i++)
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

void TempHumdityControlLoop(void)
{
  int temperature = 0, humidity = 0, status = 0;
  int FailCount = 0;
  int SetCold = 0;

  status = ReadSensor(&temperature, &humidity);
  printf("Gathering Initial Temperature and Humidity Readings \n");
  if(status == 0) {
    printf("Temperature: %d\n", temperature);
    printf("Humidity: %d\n", humidity);
  }
  else {
    printf("Reading FAILED, exiting control loop \n");
    return;
  }
  delay(60000);
  
  while(1)
  {
    status = ReadSensor(&temperature, &humidity);
    switch(status)
    {
       case 0:
         // Good case, proceed here
         printf("Read Temperature %d and Humidity %d\n", temperature, humidity);
         FailCount = 0;
         break;
       case -1:
       case -2:
       default:
         FailCount++;
         printf("Reading FAILED, waiting and trying again. Incrementing fail count to %d. Waiting... \n", FailCount);
         delay(60000);
         continue;
    }

    if((temperature > SET_TEMP) || (humidity > MAX_HUMIDITY))
    {
      SetCold = 1;
    }
    else
    {
      SetCold = 0;
    }

    // Dont freeze the room and collect extra humidity
    if(temperature < MIN_TEMP)
    {
      SetCold = 0;
    }

    if(SetCold == 1)
    {
      TransmitPattern(SignalOnDehumidify, SIGNAL_ON_DEHUMIDIFY_LENGTH);
      printf("SENT cold signal \n");
    }
    else
    {
      TransmitPattern(SignalOff, SIGNAL_OFF_LENGTH);
      printf("SENT off signal \n");
    }

    // Re evaluate position every 5 minutes
    delay(60000 * 5);
  }
}


