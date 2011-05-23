/* stegano.c */

#include "types.h"
#include "stegano.h"

#include "macros.h"

#define BITS_PER_BYTE 8
#define BLOCK_SIZE 2

stegResult_t stegEmbed(void *carrier, long carrierSize, void *message, long messageSize, stegMode_t mode) {
	return stegResult_fail;

}


static stegResult_t stegEmbedLSB(byte *carrier, long carrierSize, byte *message, long messageSize){

	long bits = getSizeInBits(messageSize);
	long i;

	for(i = 0; i < bits ; i++){
		BIT_CLEAR(carrier[i*BLOCKSIZE+1], 0);
		if(BIT_CHECK(bitArrayGet(carrier, i), 0)){
			BIT_SET(carrier[i*BLOCKSIZE+1], 1);
		}

	}

	return stegResult_Success;
}

static void stegEmbedLSBDWORD(byte *carrier, DWORD *data, long *offset){

	int bits = sizeof(DWORD)*BITS_PER_BYTE;
	int i;

	for(i = 0 ; i < bits ; i++, (*offset)++){
		BIT_CLEAR(carrier[offset*BLOCKSIZE+1], 0);
		if(BIT_CHECK(bitArrayGet(data, i), 0)){
			BIT_SET(carrier[i*BLOCKSIZE+1], 1);
		}
	}


}
