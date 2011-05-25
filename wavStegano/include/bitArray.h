/* bitArray.h */

#ifndef BITARRAY_H_
#define BITARRAY_H_

#include "types.h"

void bitArraySet(BYTE *array, long index, int value);

int bitArrayGet(BYTE *array, long index);

int bitArrayGetDWORD(DWORD value, int idx);

#endif /* BITARRAY_H_ */
