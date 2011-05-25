/* stegano.c */

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "stegano.h"
#include "bitArray.h"

#define DEBUG_LOG
#include "debug.h"

#include "macros.h"

#define BITS_PER_BYTE 8
#define BLOCK_SIZE 2


static int getPayloadSize(BYTE *carrier);

static void writePayloadSize(BYTE *carrier, int size);
static void writePayloadData(BYTE *carrier, BYTE *payload, long bits);
static void writePayloadExtention(int dataOffset, BYTE* carrier, char *extention);

static stegResult_t stegEmbedLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention);
static stegResult_t	stegExtractLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention);

stegResult_t stegEmbed(dataHolder_t *carrier, dataHolder_t *payload, stegMode_t mode, char *extention) {

	switch (mode) {
	case stegMode_LSB:
		return stegEmbedLSB(carrier, payload, extention);
		break;
	case stegMode_LSB4:
	case stegMode_LSBE:
		return stegResult_fail;

	}
	return stegResult_fail;
}

stegResult_t stegExtract(dataHolder_t *carrier, dataHolder_t *payload, stegMode_t mode, char *extention) {
	switch (mode) {
	case stegMode_LSB:
		return stegExtractLSB(carrier, payload, extention);
		break;
	case stegMode_LSB4:
	case stegMode_LSBE:
		return stegResult_fail;

	}
	return stegResult_fail;
}

static stegResult_t stegEmbedLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {

	LOG("stegEmbedLSB((%x, %d), (%x, %d), %s)\n", carrier->data, carrier->size, payload->data, payload->size, STR_NULL(extention));
	long bits = BITS_PER_BYTE*payload->size;
	LOG("data bits: %ld\n", bits);

	writePayloadSize(carrier->data, payload->size);

	writePayloadData(carrier->data, payload->data, bits);

	//if(extention != NULL){
	//	writePayloadExtention(sizeof(DWORD)+(payload->size), carrier->data, extention);
	//}


	return stegResult_Success;
}

static stegResult_t stegExtractLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	int sizeOffset = BITS_PER_BYTE * sizeof(DWORD);
	long i;
	int size;


	LOG("reading payloadSize\n");
	size = getPayloadSize(carrier->data);
	LOG("payload size: %d\n", size);

	if ((payload->data = (BYTE*) malloc(size)) == NULL) {
		return stegResult_memoryFail;
	}

	for (i = sizeOffset; i < size * BITS_PER_BYTE; i++) {
		bitArraySet(payload->data, i, BIT_GET(carrier->data[i*BLOCK_SIZE+1], 0));
		printf("%d", BIT_GET(carrier->data[i*BLOCK_SIZE+1], 0));
	}
	putchar('\n');
	return stegResult_Success;

}

static void writePayloadSize(BYTE *carrier, int size){
	LOG("writePayloadSize(%p, %d)\n", carrier, size);
	int i;

	for(i = 0 ; i < sizeof(DWORD)*BITS_PER_BYTE ; i++){
		if(bitArrayGetDWORD(size, i)){
			carrier[i*BLOCK_SIZE+1] |= 0x1;
			printf("1");
		} else {
			carrier[i*BLOCK_SIZE+1] &= ~0x1;
			printf("0");
		}
	}
	putchar('\n');
}

static void writePayloadData(BYTE *carrier, BYTE *payload, long bits) {
	LOG("writePayloadData(%p, %p, %ld)\n", carrier, payload, bits);
	long sizeOffset = BITS_PER_BYTE * sizeof(DWORD);
	LOG("sizeOffset: %ld\n", sizeOffset);
	long i;

	for (i = 0; i < bits; i++) {
		if (bitArrayGet(payload, i)) {
			carrier[(i+sizeOffset)*BLOCK_SIZE+1] |= 0x1; //Set bit
			printf("1");
		} else {
			carrier[(i+sizeOffset)*BLOCK_SIZE+1] &= ~0x1; //Clear bit
			printf("0");
		}
	}
	putchar('\n');
}

static void writePayloadExtention(int dataOffset, BYTE* carrier, char *extention){
	int len = strlen(extention) + 1;
	int lenBits = len*BITS_PER_BYTE;
	long i;

	for(i = 0; i < lenBits ; i++, dataOffset++){
		if(bitArrayGet((BYTE*)extention, i)) {
			BIT_SET(carrier[dataOffset*BLOCK_SIZE+1], 0);
		} else {
			BIT_CLEAR(carrier[dataOffset*BLOCK_SIZE+1], 0);
		}
	}
}

static int getPayloadSize(BYTE *carrier) {
	LOG("getPayloadSize(%p)\n", carrier);
	int i;
	int ret = 0;

	for(i = 0 ; i < sizeof(DWORD)*BITS_PER_BYTE*2 ; i++){
		printf("%x ", carrier[i]);
	}


	for(i = 0 ; i < sizeof(DWORD)*BITS_PER_BYTE ; i++){
		if(carrier[i*BLOCK_SIZE+1] & 0x1){
			BIT_SET(ret, sizeof(DWORD)*BITS_PER_BYTE - (i + 1));
			LOG("bit %d is 1\n", sizeof(DWORD)*BITS_PER_BYTE - (i + 1));
		} else {
			LOG("bit %d is 0\n", sizeof(DWORD)*BITS_PER_BYTE - (i + 1));
		}
	}
	return ret;
}

