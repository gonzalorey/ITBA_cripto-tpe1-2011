/*
 * wav.c
 *
 *  Created on: Apr 15, 2011
 */

#include <stdio.h>

#include "wav.h"
#include "types.h"
#include "debug.h"

typedef DWORD FOURCC; //Four-character code
typedef FOURCC CKID;	//Four-character-code chunk identifier
typedef DWORD CKSIZE; //32-bit unsigned size value

typedef struct {
	CKID	chunkID;
	CKSIZE	chunkSize;
	CKID	format;
} RIFF_CK;


typedef struct {
	CKID	chunkID;
	CKSIZE	chunkSize;
	BYTE	*soundData;
} DATA_CK;

typedef struct {
	CKID	chunkID;
	CKSIZE	chunkSize;
	/* Common fields */
	WORD	wFormatTag; // Format category,i.e.:PCM = 1 (no compres.)
	WORD	wChannels; // Number of channels:1, mono; 2, stereo
	DWORD	dwSamplesPerSec;	// Sampling rate: Mhz DWORD	dwAvgBytesPerSec;
	WORD	wBlockAlign;
	WORD	wBitsPerSample; //8, 16, etc.
	WORD	extraParamSize;// If PCM, doesn't exist
	BYTE	*extraParams;//space for extra params
}FMT_CK;

typedef struct {
	RIFF_CK	riff_desc;
	FMT_CK	fmt;
	DATA_CK	data;
} wavCDT;

static wav_t loadWav(FILE *source);

wav_t newWavFromPath(char *filepath){
	LOG("in newWav\n");
	FILE *source;

	if((source = fopen(filepath, "r")) == NULL){
		ERROR("File open failed\n");
		return NULL;
	}

	return loadWav(source);

}

static wav_t loadWav(FILE *source){
	LOG("load wav\n");

	wav_t wav;

	if((wav = malloc(sizeof(wavCDT))) == NULL)
		return NULL;

	CKID ckId;
	int bytes;
	if ((bytes = fread(&ckId, sizeof(DWORD), 1, source)) == 0){
		FATAL("Cantidad de bytes leidos: %d\n", bytes);
		return NULL;
	}

	char c1, c2, c3, c4;
	c1 = ((char*)&ckId)[0];
	c2 = ((char*)&ckId)[1];
	c3 = ((char*)&ckId)[2];
	c4 = ((char*)&ckId)[3];

	LOG("ckId: %c%c%c%c\n", c1, c2, c3, c4);


	return NULL;


}
