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

#define MAX_FILE_EXTENTION 8

static int getPayloadSize(BYTE *carrier);

static void writePayloadSizeLSB(BYTE *carrier, int size);
static void writePayloadDataLSB(BYTE *carrier, int offset, BYTE *payload, long bits);

static char getByteLSB(int *offset, BYTE *carrier);


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

	LOG("stegEmbedLSB((%p, %d), (%p, %d), %s)\n", carrier->data, carrier->size, payload->data, payload->size, STR_NULL(extention));
	long bits = BITS_PER_BYTE*payload->size;
	LOG("data bits: %ld\n", bits);

	writePayloadSizeLSB(carrier->data, payload->size);

	writePayloadDataLSB(carrier->data, sizeof(DWORD)*BITS_PER_BYTE,  payload->data, bits);

	if(extention != NULL){
		writePayloadDataLSB(carrier->data, (sizeof(DWORD) + payload->size)*BITS_PER_BYTE,(BYTE*) extention, (strlen(extention)+1)*BITS_PER_BYTE);
	}


	return stegResult_Success;
}

static stegResult_t stegExtractLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	int sizeOffset = BITS_PER_BYTE * sizeof(DWORD);
	long i;
	int size;


	LOG("reading payloadSize\n");
	size = getPayloadSize(carrier->data);
	payload->size = size;
	LOG("payload size: %d\n", size);

	if ((payload->data = (BYTE*) malloc(size)) == NULL) {
		return stegResult_memoryFail;
	}

	for (i = 0; i < size * BITS_PER_BYTE; i++) {
		bitArraySet(payload->data, i, carrier->data[(i+sizeOffset)*BLOCK_SIZE+1] & 0x1);
	}

	if(extention != NULL){
		int offset = (sizeof(DWORD)+payload->size)*BITS_PER_BYTE;
		char val;
		i = 0;
		if((val = getByteLSB(&offset, carrier->data)) != '.'){
			WARN("Looking for extention, but '.' is not there...\n");
			free(payload->data);
			payload->data = NULL;
			return stegResult_fail;
		} else {
			extention[i++] = val;
			do {
				val = getByteLSB(&offset, carrier->data);
				extention[i++] = val;
			}while(val != 0 && i < MAX_FILE_EXTENTION);

			if(val != 0){
				WARN("Not found Null termination in extention\n");
				free(payload->data);
				payload->data = NULL;
				return stegResult_fail;
			}
		}

	}


	return stegResult_Success;

}

static char getByteLSB(int *offset, BYTE *carrier) {
	char ret = 0;
	int bits = sizeof(char)*BITS_PER_BYTE;
	int i;
	for(i = 0 ; i < bits ; i++){
		if(carrier[(i+*offset)*BLOCK_SIZE+1] & 0x1){
			bitArraySetBYTE((BYTE*)&ret, i, 1);
		}
	}
	*offset += i;
	return ret;
}

static void writePayloadSizeLSB(BYTE *carrier, int size){
	LOG("writePayloadSize(%p, %d)\n", carrier, size);
	int i;

	for(i = 0 ; i < sizeof(DWORD)*BITS_PER_BYTE ; i++){
		if(bitArrayGetDWORD(size, i)){
			carrier[i*BLOCK_SIZE+1] |= 0x1;
		} else {
			carrier[i*BLOCK_SIZE+1] &= ~0x1;
		}
	}
}

static void writePayloadDataLSB(BYTE *carrier, int offset, BYTE *payload, long bits) {
	LOG("writePayloadData(%p, %d, %p, %ld)\n", carrier, offset, payload, bits);
	LOG("sizeOffset: %d\n", offset);
	long i;

	for (i = 0; i < bits; i++) {
		if (bitArrayGet(payload, i)) {
			carrier[(i+offset)*BLOCK_SIZE+1] |= 0x1; //Set bit
		} else {
			carrier[(i+offset)*BLOCK_SIZE+1] &= ~0x1; //Clear bit
		}
	}
}

static int getPayloadSize(BYTE *carrier) {
	LOG("getPayloadSize(%p)\n", carrier);
	int i;
	int ret = 0;

	for(i = 0 ; i < sizeof(DWORD)*BITS_PER_BYTE ; i++){
		if(carrier[i*BLOCK_SIZE+1] & 0x1){
			BIT_SET(ret, sizeof(DWORD)*BITS_PER_BYTE - (i + 1));
		} else {
		}
	}
	return ret;
}
