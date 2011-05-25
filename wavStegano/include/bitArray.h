/* bitArray.h */

#ifndef BITARRAY_H_
#define BITARRAY_H_

#include "types.h"

void bitArraySet(BYTE *array, long index, int value);

int bitArrayGet(BYTE *array, long index);

int bitArrayGetDWORD(DWORD value, int idx);

void bitArraySetBYTE(BYTE *value, int idx, int flag);

int getFourLeastSignificantDWORD(DWORD value, int byte);

int getFourMostSignificantDWORD(DWORD value, int byte);

int bitArrayGetFourLeastSignificant(BYTE * array, int byte);

int bitArrayGetFourMostSignificant(BYTE * array, int byte);

#endif /* BITARRAY_H_ */
