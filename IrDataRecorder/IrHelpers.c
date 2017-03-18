#include <stdio.h>
#include "IrHelpers.h"

int ValueIsWithinPercent(long int inval, long int protovalue, int percent)
{
  double upper;
  double lower;
  double modifiedpercent;

  modifiedpercent = 1.0 + (percent/100.0);
  upper = protovalue*modifiedpercent;
//  printf("ValueIsWithinPercent: Upper %f Percent %f\n", upper, modifiedpercent);

  modifiedpercent = 1.0 - (percent/100.0);
  lower = protovalue*modifiedpercent;
//  printf("ValueIsWithinPercent: Lower %f Percent %f\n", lower, modifiedpercent);

  if ((inval > lower ) && (inval < upper))
  {
    return 1;
  }

  return 0;
}


int partition(unsigned long int a[], int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;

   while( 1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}

void quickSort(unsigned long int a[], int l, int r)
{
   int j;

   if( l < r )
   {
   	// divide and conquer
        j = partition( a, l, r);
       quickSort( a, l, j-1);
       quickSort( a, j+1, r);
   }
}


void PrintBitArray(unsigned int ba [], int arrayLength)
{
  int i, j;

  for(i=0; i<arrayLength; i+=8)
  {
    if((arrayLength - i) > 8)
    {
      for(j = 0; j < 8; j++)
      {
        printf("%d ,", ba[i+j]);
      }
    }
    else
    {
      for(j = 0; j < (arrayLength - i); j++)
      {
        printf("%d ,", ba[i+j]);
      }
    }
    printf("\n");
  }
}

//
// print Array
//
void PrintArray(unsigned long int times[], unsigned long int times2[], int ArrayLength)
{
  int i;
  for(i = 0; i < ArrayLength; i++)
  {
     printf("%lu                ", times[i]);
     if(times2 != NULL)
        printf("%lu\n", times2[i]);
     else
        printf("\n");
  }
}


void TranslateBitsToString (char HexValue[], unsigned int Stack[], int bits)
{
  int passes;
  unsigned int val;
  int i;
  passes = (bits/4) + 1;

  printf("Need space for %d bits \n", bits);
  printf("%d passes\n", passes);
  for(i =0; i < passes; i++)
  {
    val = 0;
    if( Stack[i*4 + 0] > 0)
	val += 8;
    if( Stack[i*4 + 1] > 0)
        val += 4;
    if( Stack[i*4 + 2] > 0)
        val += 2;
    if( Stack[i*4 + 3] > 0)
        val += 1;

//    printf("Val %d is %d \n", i, val);
    switch (val)
    {
      	case 0:
   HexValue[i] = '0';
   break;
   case 1:
   HexValue[i] = '1';
   break;
   case 2:
   HexValue[i] = '2';
   break;
   case 3:
   HexValue[i] = '3';
   break;
   case 4:
   HexValue[i] = '4';
   break;
   case 5:
   HexValue[i] = '5';
   break;
   case 6:
   HexValue[i] = '6';
   break;
   case 7:
   HexValue[i] = '7';
   break;
   case 8:
   HexValue[i] = '8';
   break;
   case 9:
   HexValue[i] = '9';
   break;
   case 10:
   HexValue[i] = 'A';
   break;
   case 11:
   HexValue[i] = 'B';
   break;
   case 12:
   HexValue[i] = 'C';
   break;
   case 13:
   HexValue[i] = 'D';
   break;
   case 14:
   HexValue[i] = 'E';
   break;
   case 15:
   HexValue[i] = 'F';
   break;
   default:
   HexValue[i] = 'X';
   break;
    }
  }

  HexValue[i] = '\0';
}

// First value of array is 0. Keep that in mind when analyzing
void FlattenTimeArray(unsigned long int times[], int ArrayLength)
{
  int i;
  unsigned long int PrevValue, CurrentValue;

  PrevValue = times[0];
  times[0] = 0;
  for(i = 1; i < ArrayLength; i++)
  {
     CurrentValue = times[i];
     if(times[i] > PrevValue)
     {
       times[i] = times[i] - PrevValue;
     }
    else
    {
      // The clock has rolled over!
      times[i] = times[i] + (1000000000 - PrevValue);
    }
    PrevValue = CurrentValue;
  }
}
