/* bitArray.c */

#include "bitArray.h"
#include "types.h"
#include "macros.h"

void bitArraySet(BYTE *array, long index, int value) {

	int byteIndex = index/8;
	int bitIndex = index % 8;

	if(value) {
		array[byteIndex] |= (0x80 >> bitIndex);
	} else {
		array[byteIndex] &= ~(0x80 >> bitIndex);
	}
}

int bitArrayGet(BYTE *array, long index) {
	int byteIndex = index/8;
	int bitIndex = index % 8;

	return 	array[byteIndex] & (0x80 >> bitIndex);
}

int bitArrayGetDWORD(DWORD value, int idx){
	return value & (0x80000000 >> idx);
}

void bitArraySetBYTE(BYTE *value, int idx, int flag){
	if (flag) {
		*value |= (0x80 >> idx);
	} else {
		*value &= ~(0x80 >> idx);
	}
}

int bitArrayGetFourLeastSignificantDWORD(DWORD value, int byte){
	return (value >> (byte * 8)) & 0xF;
}

int bitArrayGetFourMostSignificantDWORD(DWORD value, int byte){
	return (value >> (byte * 8 + 4)) & 0xF;
}

int bitArrayGetFourLeastSignificant(BYTE * array, int byte){
	return array[byte] & 0xF;
}

int bitArrayGetFourMostSignificant(BYTE * array, int byte){
	return (array[byte] >> 4) & 0xF;
}
