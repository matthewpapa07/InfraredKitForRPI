
#ifndef IRHELPERS_H
#define IRHELPERS_H

#include "IrCommon.h"

int ValueIsWithinPercent(long int inval, long int protovalue, int percent);


void quickSort(unsigned long int a[], int l, int r);


void PrintBitArray(unsigned int ba [], int arrayLength);

//
// print Array
//
void PrintArray(unsigned long int times[], unsigned long int times2[], int ArrayLength);

void TranslateBitsToString (char HexValue[], unsigned int Stack[], int bits);

// First value of array is 0. Keep that in mind when analyzing
void FlattenTimeArray(unsigned long int times[], int ArrayLength);

#endif /*IRHELPERS_H*/
