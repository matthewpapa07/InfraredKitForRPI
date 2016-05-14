#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define PWM_PIN 18

int main (int argc, char *argv[])
{
//  int i;

  wiringPiSetupGpio(); // Broadcomm GPIO Pinout

  pinMode(PWM_PIN, PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);

  // pwmFrequency in Hz = 19.2e6 Hz / pwmClock (freq divisor) / pwmRange (counter that increments).
  // SetRange is 2 to 4095
  //
  //pwmWrite(1, 75);  //theretically 50 (1ms) to 100 (2ms) on my servo 30-130 works okpinMode(1, PWM_OUTPUT);

  pwmSetClock(50); //Clock divided by this value is how fast the range counter increments
  pwmSetRange(10); // Half of the counter low half high
  pwmWrite(PWM_PIN, 5);  //theretically 50 (1ms) to 100 (2ms) on my servo 30-130 works okpinMode(1, PWM_OUTPUT);



/*
  printf("Start Program\n");
  for(i=0; i < 3000; i++)
  {
    digitalWrite(4, 0);
    usleep(1000);
    digitalWrite(4, 1);
    usleep(1000);
  }
  printf("End Program\n");
*/
  return 0;
}

/*

I've recently had some reason to start experimenting with PWM myself, and found that (as pointed out by one of the comments) the frequency seems to vary with duty cycle - bizzare, right? It turns out that Broadcom implemented "balanced" PWM in order to make the on and off PWM pulses as evenly distributed as possible. They give a description of the algorithm and some more discussion on page 139 of their datasheet: http://www.element14.com/community/servlet/JiveServlet/downloadBody/43016-102-1-231518/Broadcom.Datasheet.pdf

So what you really want is to put PWM in mark-space mode, which will give you the traditional (and easily predictable) PWM you're looking for:

pwmSetMode(PWM_MODE_MS);

The rest of the answer assumes we're in mark-space mode.

I also did some experimenting with the allowable range of values for pwmSetClock() and pwmSetRange(). As noted in one of the other answers, the valid range for pwmSetClock() seems to go from 2 to 4095, while the valid range for pwmSetRange() is up to 4096 (I didn't attempt to find a lower-limit).

The range and clock (a better name is probably divisor) both affect the frequency. The range also affects resolution, so while it may be possible to use very low values, there is a practical limit to how low you will probably want to go. For example, if you used a range of 4, you could achieve higher frequencies, but you will only be able to set the duty cycle to 0/4, 1/4, 2/4, 3/4 or 4/4.

The Raspberry Pi PWM clock has a base frequency of 19.2 MHz. This frequency, divided by the argument to pwmSetClock(), is the frequency at which the PWM counter is incremented. When the counter reaches a value equal to the specified range, it resets to zero. While the counter is less than the specified duty cycle, the output is high, otherwise the output is low.

This means, if you want to set the PWM to have a specific frequency, you can use the following relationship:

pwmFrequency in Hz = 19.2e6 Hz / pwmClock / pwmRange.

If you use the maximum permissible values for pwmSetClock() and pwmSetRange(), you will end up with the minimum achievable hardware PWM frequency of ~1.14 Hz. This will certainly give a visible flicker (more of a flash, really) to an LED. I did confirm the above equation with an oscilloscope, and it seems to hold. The upper frequency limit will be affected by the resolution you need, as described above.
*/
