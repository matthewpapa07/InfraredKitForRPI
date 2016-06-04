#include "TemperatureControl.h"

#include <math.h>

//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
    return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
    // (1) Saturation Vapor Pressure = ESGG(T)
    double RATIO = 373.15 / (273.15 + celsius);
    double RHS = -7.90298 * (RATIO - 1);
    RHS += 5.02808 * log10(RATIO);
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
    RHS += log10(1013.246);

    // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;

    // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP/0.61078);   // temp var
    return (241.88 * T) / (17.558 - T);
}

int ReadSensor(int *temperature, int *humidity)
{
  // BUFFER TO RECEIVE
  int bits[5];
  int cnt = 7;
  int idx = 0;
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
      if (loopCnt-- == 0) 
        return -1;
 
 
    unsigned long t = micros();
    loopCnt = 10000;

    while(digitalRead(SENSOR_PIN) == HIGH)
      if (loopCnt-- == 0) 
        return -1;
 
    if ((micros() - t) > 40) 
      bits[idx] |= (1 << cnt);
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

  int sum = bits[0] + bits[2];  

  //Check for checksum error
  if (bits[4] != sum) 
    return -2;

  return 0;
}


