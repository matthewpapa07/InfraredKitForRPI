#include "IrCommon.h"



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

void Transmit(char HexString[], int bits, int baud)
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
  for(i = 0; i < bits; i++)
  {
    //ChangePwm(OutBits[i]); //TODO: Change
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

void SendMessage (void)
{
//  wiringPiSetupGpio(); // Broadcomm GPIO Pinout
  wiringPiSetup(); // Wiring pi simplified GPIO pinout

  pinMode(PWM_PIN, PWM_OUTPUT);

  //
  // set up 38kHz carrier wave
  //
  pwmSetMode(PWM_MODE_MS);
  pwmSetClock(50); //Clock divided by this value is how fast the range counter increments
  pwmSetRange(10); // Half of the counter low half high

  ChangePwm(0);
}
