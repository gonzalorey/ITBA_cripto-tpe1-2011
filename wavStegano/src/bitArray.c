/* bitArray.c */

#include "bitArray.h"
#include "types.h"
#include "macros.h"

void bitArraySet(BYTE *array, long index, int value) {

	int byteIndex = index/8;
	int bitIndex = index % 8;

	if(value) {
		BIT_SET(array[byteIndex], bitIndex);
	} else {
		BIT_CLEAR(array[byteIndex], bitIndex);
	}
}

int bitArrayGet(BYTE *array, long index) {
	int byteIndex = index/8;
	int bitIndex = index % 8;

	return 	array[byteIndex] & (0x8 >> bitIndex);
}
