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
static int getPayloadDataLSB4(BYTE * carrier, BYTE ** payload, int offset, int size);
static int getPayloadExtentionLSB4(BYTE * carrier, char ** extention, int offset);
static char getByteLSB4(int *offset, BYTE *carrier);

stegResult_t stegEmbed(dataHolder_t *carrier, dataHolder_t *payload, stegMode_t mode, char *extention) {

	switch (mode) {
	case stegMode_LSB:
		return stegEmbedLSB(carrier, payload, extention);
	case stegMode_LSB4:
		return stegEmbedLSB4(carrier, payload, extention);
	case stegMode_LSBE:
		return stegEmbedLSBEnhanced(carrier, payload, extention);
	case stegMode_Wanted:
		printf("Not implemented\n");
	case stegMode_none:
		break;
	}
	return stegResult_fail;
}

stegResult_t stegExtract(dataHolder_t *carrier, dataHolder_t *payload, stegMode_t mode, char *extention) {
	switch (mode) {
	case stegMode_LSB:
		return stegExtractLSB(carrier, payload, extention);
		break;
	case stegMode_LSB4:
		return stegExtractLSB4(carrier, payload, extention);
		break;
	case stegMode_LSBE:
		return stegExtractLSBEnhanced(carrier, payload, extention);
	case stegMode_Wanted:
	case stegMode_none:
		break;
	}
	return stegResult_fail;
}

static stegResult_t stegEmbedLSB(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {

	if((payload->size+ MAX_FILE_EXTENTION)*BITS_PER_BYTE*BLOCK_SIZE > carrier->size){
		printf("Payload can't be hold in the carrier. Carrier too small\n");
		return stegResult_fail;
	}

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
	printf("payload size is %d\n", payload->size);
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

/******************************************************************************************************************
 * LSB ENHANCED
 ******************************************************************************************************************/
static stegResult_t stegEmbedLSBEnhanced(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	long bits = BITS_PER_BYTE*payload->size;
	int offset = 0;

	if (!writePayloadSizeLSBEnhanced(carrier->data, &offset, carrier->size, payload->size)) {
		ERROR("PayloadSize wrongly hidden!\n");
		return stegResult_sizeFail;
	}

	if (!writePayloadDataLSBEnhanced(carrier->data, &offset, carrier->size, payload->data, bits)) {
		ERROR("PayloadData wrongly hidden!\n");
		ERROR("The carrier could not have enough space for hidding the payload!\n");
		return stegResult_fail;
	}

	if(extention != NULL) {
		if(!writePayloadDataLSBEnhanced(carrier->data, &offset, carrier->size, (BYTE*) extention, (strlen(extention)+1)*BITS_PER_BYTE)) {
			ERROR("Extention wrongly hidden!\n");
			return stegResult_fail;
		}
	}

	return stegResult_Success;
}

static int getPayloadSizeEnhanced(dataHolder_t *carrier, int * offset) {
	LOG("getPayloadSize(%p)\n", carrier->data);
	int i, j = 0;
	int ret = 0;

	for(i = *offset ; i < carrier->size && j != sizeof(DWORD)*BITS_PER_BYTE ; i++) {
		if (carrier->data[i] == 0xFF || carrier->data[i] == 0xFE) {
			if (carrier->data[i] & 0x1) {
				BIT_SET(ret, sizeof(DWORD)*BITS_PER_BYTE - (j + 1));
			}
			j++;
		}
	}

	*offset = i;
	LOG("ret: %d\n", ret);
	printf("payload size is %d\n", ret);
	return ret;
}

static BYTE * getPayloadDataEnhanced (dataHolder_t *carrier, int * offset, int payloadSize) {
	LOG("getPayloadSizeEnhanced: (%d, %d)\n", *offset, payloadSize);
	int i, j = 0;
	BYTE * data;

	if ((data = calloc (1, sizeof(BYTE) * payloadSize)) == NULL) {
		return NULL;
	}

	for(i = *offset ; i < carrier->size && j != payloadSize*sizeof(BYTE)*BITS_PER_BYTE ; i++) {
		if (carrier->data[i] == 0xFF || carrier->data[i] == 0xFE) {
			if (carrier->data[i] & 0x1) {
				bitArraySet(data, j, 1);
			}
			j++;
		}
	}

	*offset = i;
	return data;
}

static char getByteLSBEnhanced(int *offset, dataHolder_t *carrier) {
	char ret = 0;
	int bits = sizeof(char)*BITS_PER_BYTE, i, j = 0;
	for (i = *offset; i < carrier->size && j < bits; i++) {
		if (carrier->data[i] == 0xFE || carrier->data[i] == 0xFF) {
			if (carrier->data[i] & 0x1) {
				bitArraySetBYTE((BYTE*)&ret, j, 1);
			}
			j++;
		}
	}
	*offset = i;
	return ret;
}

static stegResult_t	stegExtractLSBEnhanced(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	long i;
	int size, offset = 0;

	LOG("reading payloadSize\n");
	size = getPayloadSizeEnhanced(carrier, &offset);
	payload->size = size;
	LOG("payload size: %d\n", size);

	if ((payload->data = getPayloadDataEnhanced (carrier, &offset, payload->size)) == NULL) {
		return stegResult_memoryFail;
	}

	if(extention != NULL){
		char val;
		i = 0;
		if((val = getByteLSBEnhanced(&offset, carrier)) != '.'){
			WARN("Looking for extention, but '.' is not there...\n");
			free(payload->data);
			return stegResult_fail;
		} else {
			extention[i++] = val;
			do {
				val = getByteLSBEnhanced(&offset, carrier);
				extention[i++] = val;
			} while(val != 0 && i < MAX_FILE_EXTENTION);

			if(val != 0){
				WARN("Not found Null termination in extention\n");
				free(payload->data);
				return stegResult_fail;
			}
		}

	}

	return stegResult_Success;
}

static int writePayloadSizeLSBEnhanced(BYTE *carrier, int * offset, int carrierSize, int size) {
	LOG("writePayloadSize(%p, %X)\n", carrier, size);
	long i, j = 0;

	for (i = *offset; i < carrierSize && j != sizeof(DWORD)*BITS_PER_BYTE; i++) {
		if (carrier[i] == 0xFE || carrier[i] == 0xFF) {
			if (bitArrayGetDWORD(size, j)) {
				carrier[i] |= 0x1;
			} else {
				carrier[i] &= ~0x1;
			}
			j++;
		}
	}

	if (i == carrierSize) {
		/* El payload no entra en el carrie */
		return 1;
	}

	*offset = i;
	/* Si se pudieron escribir los 32 bits devuelve 1 ya que se oculto satisfactoriamente la longitud del
	 * payload dentro del carrier.
	 */
	return (j == sizeof(DWORD)*BITS_PER_BYTE);
}

static int writePayloadDataLSBEnhanced(BYTE *carrier, int * offset, int carrierSize, BYTE *payload, long bits) {
	LOG("writePayloadDataLSBEnhanced(%p, %d, %p, %ld)\n", carrier, *offset, payload, bits);
	LOG("sizeOffset: %d\n", *offset);
	long i, j = 0;

	for (i = *offset; i < carrierSize && j != bits; i++) {
		if (carrier[i] == 0xFE || carrier[i] == 0xFF) {
			if (bitArrayGet(payload, j)) {
				carrier[i] |= 0x1; //Set bit
			} else {
				carrier[i] &= ~0x1; //Clear bit
			}
			j++;
		}
	}

	if (i == carrierSize) {
		/* El payload no entra en el carrier */
		return 0;
	}

	*offset = i;
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


	if((payload->size+ MAX_FILE_EXTENTION)*BITS_PER_BYTE/BITS_PER_LSB4*BLOCK_SIZE > carrier->size){
		printf("Payload can't be hold in the carrier. Carrier too small\n");
		return stegResult_fail;
	}


	writePayloadSizeLSB4(carrier->data, payload->size);

	writePayloadDataLSB4(carrier->data, sizeof(DWORD) * BITS_PER_BYTE / BITS_PER_LSB4,  payload->data, bits);

	if(extention != NULL){
		writePayloadDataLSB4(carrier->data, (sizeof(DWORD) + payload->size) * BITS_PER_BYTE / BITS_PER_LSB4,(BYTE*) extention, (strlen(extention)+1)*BITS_PER_BYTE);
	}

	LOG("steg embed successful\n");

	return stegResult_Success;
}

static stegResult_t stegExtractLSB4(dataHolder_t *carrier, dataHolder_t *payload, char *extention) {
	int sizeOffset;
	int dataOffset;
	int size;

	LOG("reading payloadSize\n");
	size = getPayloadSizeLSB4(carrier->data);
	payload->size = size;
	printf("payload size is %d\n", payload->size);
	LOG("payload size: %d\n", size);

	LOG("reading payload data\n");
	sizeOffset = sizeof(DWORD) * BITS_PER_BYTE / BITS_PER_LSB4;
	if(getPayloadDataLSB4(carrier->data, &payload->data, sizeOffset, size) != stegResult_Success) {
		return stegResult_memoryFail;
	}

	LOG("reading payload extention\n");
	dataOffset = size * BITS_PER_BYTE / BITS_PER_LSB4;
	if(extention != NULL) {
		if(getPayloadExtentionLSB4(carrier->data, &extention, sizeOffset + dataOffset) != stegResult_Success){
			free(payload->data);
			payload->data = NULL;
			return stegResult_fail;
		}
	}

	LOG("steg extract successful\n");

	return stegResult_Success;
}

static void writePayloadSizeLSB4(BYTE *carrier, int size){
	LOG("writePayloadSizeLSB4(%p, %d)\n", carrier, size);
	int i;

	for(i = 0 ; i < sizeof(DWORD) ; i++){

		// clear the 4 least significant bits of the block
		carrier[(2*i)*BLOCK_SIZE+1] &= ~0xF;

		// set the 4 least significant bits of the block with the four most significative from size
		carrier[(2*i)*BLOCK_SIZE+1] |= bitArrayGetFourMostSignificantDWORD(size, sizeof(DWORD) - i - 1);

		LOG("Most:%#02X - i:%d - offset:%d\n", bitArrayGetFourMostSignificantDWORD(size, sizeof(DWORD) - i - 1), (int)(sizeof(DWORD) -  i - 1), (2*i)*BLOCK_SIZE+1);

		// clear the 4 least significant bits of the block
		carrier[(2*i+1)*BLOCK_SIZE+1] &= ~0xF;

		// set the 4 least significant bits of the block with the four least significative from size
		carrier[(2*i+1)*BLOCK_SIZE+1] |= bitArrayGetFourLeastSignificantDWORD(size, sizeof(DWORD) - i - 1);

		LOG("Least:%#02X - i:%d - offset:%d\n", bitArrayGetFourLeastSignificantDWORD(size, sizeof(DWORD) - i - 1), (int)(sizeof(DWORD) -  i - 1), (2*i+1)*BLOCK_SIZE+1);
	}
}

static void writePayloadDataLSB4(BYTE *carrier, int offset, BYTE *payload, long bits) {
	LOG("writePayloadData(%p, %d, %p, %ld)\n", carrier, offset, payload, bits);
	LOG("sizeOffset: %d\n", offset);
	int i;

	for (i = 0; i < bits / BITS_PER_BYTE; i++) {

		// clear 4 least significant bits
		carrier[((2*i)+offset)*BLOCK_SIZE+1] &= ~0xF;

		// clear 4 most significant bits
		carrier[((2*i)+offset)*BLOCK_SIZE+1] |= bitArrayGetFourMostSignificant(payload, i);
		//LOG("Most:%#02X - i:%d - offset:%d\n", bitArrayGetFourMostSignificant(payload, i), i, ((2*i)+offset)*BLOCK_SIZE+1);

		// clear 4 least significant bits
		carrier[((2*i+1)+offset)*BLOCK_SIZE+1] &= ~0xF;

		// clear 4 most significant bits
		carrier[((2*i+1)+offset)*BLOCK_SIZE+1] |= bitArrayGetFourLeastSignificant(payload, i);
		//LOG("Least:%#02X - i:%d - offset:%d\n", bitArrayGetFourLeastSignificant(payload, i), i, ((2*i+1)+offset)*BLOCK_SIZE+1);

		//LOG("BYTE:%#02X (%c)\n", payload[i], payload[i]);
	}
}

static int getPayloadSizeLSB4(BYTE *carrier) {
	LOG("getPayloadSizeLSB4(%p)\n", carrier);
	int i;
	unsigned int ret = 0;

	for(i = 0 ; i < sizeof(DWORD) * 2; i++){
		ret <<= 4;
		ret |= bitArrayGetFourLeastSignificant(carrier, i*BLOCK_SIZE+1);
		//LOG("Least:%#02X - i:%d\n", bitArrayGetFourLeastSignificant(carrier, i*BLOCK_SIZE+1), i);
	}

	return ret;
}

static int getPayloadDataLSB4(BYTE * carrier, BYTE ** payload, int offset, int size) {
	int i;

	if ((*payload = (BYTE*) calloc(sizeof(BYTE), size)) == NULL) {
		return stegResult_memoryFail;
	}

	for (i = 0; i < size ; i++) {
		(*payload)[i] |= bitArrayGetFourLeastSignificant(carrier, (2*i + offset) * BLOCK_SIZE + 1);
		//LOG("Least:%#02X - i:%d - offset;%d\n", (int)bitArrayGetFourLeastSignificant(payload, i), i, (2*i + offset) * BLOCK_SIZE + 1);
		(*payload)[i] <<= 4;
		(*payload)[i] |= bitArrayGetFourLeastSignificant(carrier, (2*i+1 + offset) * BLOCK_SIZE + 1);
		//LOG("Least:%#02X - i:%d - offset;%d\n", (int)bitArrayGetFourLeastSignificant(payload, i), i, (2*i+1 + offset) * BLOCK_SIZE + 1);

		//LOG("BYTE:%#02X (%c)\n", (int)(*payload)[i], (*payload)[i]);
	}

	return stegResult_Success;
}

static int getPayloadExtentionLSB4(BYTE * carrier, char ** extention, int offset) {
	int i = 0;
	char val;

	if((val = getByteLSB4(&offset, carrier)) != '.'){
		WARN("Looking for extention, but '.' is not there...\n");
		return stegResult_fail;
	} else {
		(*extention)[i++] = val;
		do {
			val = getByteLSB4(&offset, carrier);
			(*extention)[i++] = val;
		}while(val != 0 && i < MAX_FILE_EXTENTION);

		if(val != 0){
			WARN("Not found Null termination in extention\n");
			return stegResult_fail;
		}
	}

	return stegResult_Success;
}

static char getByteLSB4(int *offset, BYTE *carrier) {
	unsigned char ret = 0;

	ret |= bitArrayGetFourLeastSignificant(carrier, *offset * BLOCK_SIZE + 1);
	LOG("offset:%d\n", *offset * BLOCK_SIZE + 1);
	ret <<= 4;
	ret |= bitArrayGetFourLeastSignificant(carrier, (*offset + 1) * BLOCK_SIZE + 1);
	LOG("offset:%d\n", (*offset + 1) * BLOCK_SIZE + 1);

	*offset += 2;

	LOG("ret:%#02X (%c)\n", ret, ret);

	return ret;
}
