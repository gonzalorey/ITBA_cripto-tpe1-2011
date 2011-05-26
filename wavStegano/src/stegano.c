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
#define BITS_PER_LSB4 4

#define MAX_FILE_EXTENTION 8

static int getPayloadSizeLSB(BYTE *carrier);

static void writePayloadSizeLSB(BYTE *carrier, int size);
static void writePayloadDataLSB(BYTE *carrier, int offset, BYTE *payload, long bits);

static char getByteLSB(int *offset, BYTE *carrier);

static stegResult_t stegEmbedLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention);
static stegResult_t	stegExtractLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention);

static int writePayloadSizeLSBEnhanced(BYTE *carrier, int * offset, int carrierSize, int size);
static int writePayloadDataLSBEnhanced(BYTE *carrier, int * offset, int carrierSize, BYTE *payload, long bits);

static stegResult_t stegEmbedLSBEnhanced(dataHolder_t *carrier, dataHolder_t *payload, char *extention);
static stegResult_t	stegExtractLSBEnhanced(dataHolder_t *carrier, dataHolder_t *payload, char *extention);

/*
 * LSB4
 */
static stegResult_t stegEmbedLSB4(dataHolder_t *carrier, dataHolder_t *payload, char *extention);
static stegResult_t stegExtractLSB4(dataHolder_t *carrier, dataHolder_t *payload, char *extention);

static void writePayloadSizeLSB4(BYTE *carrier, int size);
static void writePayloadDataLSB4(BYTE *carrier, int offset, BYTE *payload, long bits);
static int getPayloadSizeLSB4(BYTE *carrier);
static char getByteLSB4(int *offset, BYTE *carrier);

stegResult_t stegEmbed(dataHolder_t *carrier, dataHolder_t *payload, stegMode_t mode, char *extention) {

	switch (mode) {
	case stegMode_LSB:
		return stegEmbedLSB(carrier, payload, extention);
		break;
	case stegMode_LSB4:

	case stegMode_LSBE:
		return stegEmbedLSBEnhanced(carrier, payload, extention);
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
		return stegExtractLSBEnhanced(carrier, payload, extention);

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
	size = getPayloadSizeLSB(carrier->data);
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

static int getPayloadSizeLSB(BYTE *carrier) {
	LOG("getPayloadSize(%p)\n", carrier);
	int i;
	int ret = 0;

	for(i = 0 ; i < sizeof(DWORD)*BITS_PER_BYTE ; i++){
		if(carrier[i*BLOCK_SIZE+1] & 0x1){
			BIT_SET(ret, sizeof(DWORD)*BITS_PER_BYTE - (i + 1));
		}
	}
	return ret;
}

/**
 * LSB ENHANCED
 */
static stegResult_t stegEmbedLSBEnhanced(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	long bits = BITS_PER_BYTE*payload->size;
	int offset = 0;

	writePayloadSizeLSBEnhanced(carrier->data, &offset, carrier->size, payload->size);
	writePayloadDataLSBEnhanced(carrier->data, &offset, carrier->size, payload->data, bits);

	if(extention != NULL) {
		writePayloadDataLSBEnhanced(carrier->data, &offset, carrier->size, (BYTE*) extention, (strlen(extention)+1)*BITS_PER_BYTE);
	}

	return stegResult_Success;
}

static int getPayloadSizeEnhanced(dataHolder_t *carrier, int * offset) {
	LOG("getPayloadSize(%p)\n", carrier->data);
	int i, j = 0;
	int ret = 0;

	for(i = *offset ; i < carrier->size && j != sizeof(DWORD)*BITS_PER_BYTE ; i++) {
		if (carrier->data[i*BLOCK_SIZE] == 0xFF || carrier->data[i*BLOCK_SIZE] == 0xFE) {
			if (carrier->data[i*BLOCK_SIZE] & 0x1) {
				BIT_SET(ret, sizeof(DWORD)*BITS_PER_BYTE - (j + 1));
			}
			j++;
		}
	}
	*offset += i;
	return ret;
}

static BYTE * getPayloadDataEnhanced (dataHolder_t *carrier, int * offset, int payloadSize) {
	int i, j = 0;
	BYTE * data;

	if ((data = calloc (1, sizeof(BYTE) * payloadSize)) == NULL) {
		return NULL;
	}

	for(i = *offset ; i < carrier->size && j != sizeof(DWORD)*BITS_PER_BYTE ; i++) {
		if (carrier->data[i*BLOCK_SIZE] == 0xFF || carrier->data[i*BLOCK_SIZE] == 0xFE) {
			if (carrier->data[i*BLOCK_SIZE] & 0x1) {
				bitArraySet(data, payloadSize*BITS_PER_BYTE - (j + 1), 1);
			}
			j++;
		}
	}

	*offset += i;
	return data;
}

static char getByteLSBEnhanced(int *offset, dataHolder_t *carrier) {
	char ret = 0;
	int bits = sizeof(char)*BITS_PER_BYTE, i, j;
	for (i = 0; i < carrier->size; i++) {
		if (carrier->data[(i+(*offset))*BLOCK_SIZE] == 0xFE || carrier->data[(i+(*offset))*BLOCK_SIZE] == 0xFF) {
			for (j = 0; j < bits; j++) {
				if (carrier->data[(i+(*offset))*BLOCK_SIZE] & 0x1) {
					bitArraySetBYTE((BYTE*)&ret, j, 1);
				}
			}
		}
	}
	*offset += i;
	return ret;
}

static stegResult_t	stegExtractLSBEnhanced(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	long i;
	int size, offset = 0;

	LOG("reading payloadSize\n");
	size = getPayloadSizeEnhanced(carrier, &offset);
	payload->size = size;
	LOG("payload size: %d\n", size);

	if ((payload->data = (BYTE*) malloc(size)) == NULL) {
		return stegResult_memoryFail;
	}

	if ((payload->data = getPayloadDataEnhanced (carrier, &offset, payload->size)) == NULL) {
		return stegResult_memoryFail;
	}

	if(extention != NULL){
		char val;
		i = 0;
		if((val = getByteLSBEnhanced(&offset, carrier)) != '.'){
			WARN("Looking for extention, but '.' is not there...\n");
			free(payload->data);
			payload->data = NULL;
			return stegResult_fail;
		} else {
			extention[i++] = val;
			do {
				val = getByteLSBEnhanced(&offset, carrier);
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

static int writePayloadSizeLSBEnhanced(BYTE *carrier, int * offset, int carrierSize, int size) {
	LOG("writePayloadSize(%p, %d)\n", carrier, size);
	long i, j = 0;

	for (i = 0; i < carrierSize && j != sizeof(DWORD)*BITS_PER_BYTE; i++) {
		if (carrier[(i+(*offset))*BLOCK_SIZE] == 0xFE || carrier[(i+(*offset))*BLOCK_SIZE] == 0xFF) {
			if (bitArrayGetDWORD(size, i)) {
				carrier[i*BLOCK_SIZE] |= 0x1;
			} else {
				carrier[i*BLOCK_SIZE] &= ~0x1;
			}
			j++;
		}
	}

	if (i == carrierSize) {
		/* El payload no entra en el carrie */
		return 1;
	}

	*offset += i;
	/* Si se pudieron escribir los 32 bits devuelve 1 ya que se oculto satisfactoriamente la longitud del
	 * payload dentro del carrier.
	 */
	return (j == sizeof(DWORD)*BITS_PER_BYTE);
}

static int writePayloadDataLSBEnhanced(BYTE *carrier, int * offset, int carrierSize, BYTE *payload, long bits) {
	LOG("writePayloadDataLSBEnhanced(%p, %d, %p, %ld)\n", carrier, *offset, payload, bits);
	LOG("sizeOffset: %d\n", *offset);
	long i, j = 0;

	for (i = 0; i < carrierSize && j != bits; i++) {
		if (carrier[(i+(*offset))*BLOCK_SIZE] == 0xFE || carrier[(i+(*offset))*BLOCK_SIZE] == 0xFF) {
			if (bitArrayGet(payload, i)) {
				carrier[(i+(*offset))*BLOCK_SIZE] |= 0x1; //Set bit
			} else {
				carrier[(i+(*offset))*BLOCK_SIZE] &= ~0x1; //Clear bit
			}
			j++;
		}
	}

	if (i == carrierSize) {
		/* El payload no entra en el carrie */
		return 1;
	}

	*offset += i;
	/* Se espera que (j == bits) sea verdadero */
	return (j == bits);
}

/*********************************************************************/
/**************************  LSB4  ***********************************/
/*********************************************************************/

static stegResult_t stegEmbedLSB4(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	LOG("stegEmbedLSB4((%p, %d), (%p, %d), %s)\n", carrier->data, carrier->size, payload->data, payload->size, STR_NULL(extention));
	long bits = BITS_PER_BYTE*payload->size;
	LOG("data bits: %ld\n", bits);

	writePayloadSizeLSB4(carrier->data, payload->size);

	writePayloadDataLSB4(carrier->data, sizeof(DWORD)*BITS_PER_BYTE,  payload->data, bits);

	if(extention != NULL){
		writePayloadDataLSB4(carrier->data, (sizeof(DWORD) + payload->size)*BITS_PER_BYTE,(BYTE*) extention, (strlen(extention)+1)*BITS_PER_BYTE);
	}

	return stegResult_Success;
}

static stegResult_t stegExtractLSB4(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	int sizeOffset = BITS_PER_BYTE * sizeof(DWORD);
		long i;
		int size;


		LOG("reading payloadSize\n");
		size = getPayloadSizeLSB4(carrier->data);
		payload->size = size;
		LOG("payload size: %d\n", size);

		if ((payload->data = (BYTE*) malloc(size)) == NULL) {
			return stegResult_memoryFail;
		}

		for (i = 0; i < size ; i++) {
			payload->data[i] <<= 4;
			payload->data[i] |= bitArrayGetFourLeastSignificant(carrier->data[(i+sizeOffset)*BLOCK_SIZE+1], i);
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

static void writePayloadSizeLSB4(BYTE *carrier, int size){
	LOG("writePayloadSizeLSB4(%p, %d)\n", carrier, size);
	int i;

	for(i = 0 ; i < sizeof(DWORD) ; i++){

		// clear the 4 least significant bits of the block
		carrier[(2*i)*BLOCK_SIZE+1] &= ~0xF;

		// set the 4 least significant bits of the block with the four most significative from size
		carrier[(2*i)*BLOCK_SIZE+1] |= getFourMostSignificantDWORD(size, i);

		// clear the 4 least significant bits of the block
		carrier[(2*i+1)*BLOCK_SIZE+1] &= ~0xF;

		// set the 4 least significant bits of the block with the four least significative from size
		carrier[(2*i+1)*BLOCK_SIZE+1] |= getFourLeastSignificantDWORD(size, i);
	}
}

static void writePayloadDataLSB4(BYTE *carrier, int offset, BYTE *payload, long bits) {
	LOG("writePayloadData(%p, %d, %p, %ld)\n", carrier, offset, payload, bits);
	LOG("sizeOffset: %d\n", offset);
	long i;

	for (i = 0; i < bits / BITS_PER_BYTE; i++) {

		// clear 4 least significant bits
		carrier[((2*i)+offset)*BLOCK_SIZE+1] &= ~0xF;

		// clear 4 most significant bits
		carrier[((2*i)+offset)*BLOCK_SIZE+1] |= bitArrayGetFourMostSignificant(carrier, ((2*i)+offset)*BLOCK_SIZE+1);

		// clear 4 least significant bits
		carrier[((2*i+1)+offset)*BLOCK_SIZE+1] &= ~0xF;

		// clear 4 most significant bits
		carrier[((2*i+1)+offset)*BLOCK_SIZE+1] |= bitArrayGetFourLeastSignificant(carrier, ((2*i+1)+offset)*BLOCK_SIZE+1);
	}
}

static int getPayloadSizeLSB4(BYTE *carrier) {
	LOG("getPayloadSizeLSB4(%p)\n", carrier);
	int i;
	unsigned int ret = 0;

	for(i = 0 ; i < sizeof(DWORD); i++){
		ret <<= 4;
		ret |= getFourLeastSignificantDWORD(carrier, i);
	}

	return ret;
}

static char getByteLSB4(int *offset, BYTE *carrier) {
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
