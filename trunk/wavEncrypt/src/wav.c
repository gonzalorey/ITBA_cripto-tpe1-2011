/*
 * wav.c
 *
 *  Created on: Apr 15, 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"
#include "types.h"

#define DEBUG_WARN
#include "debug.h"


#define RIFF_CK_IDENTIFIER 0x52494646 //RIFF
#define RIFF_FORMAT_IDENTIFIER 0x57415645 //WAVE
#define FMT_CK_IDENTIFIER 0x666d7420 //fmt[]
#define DATA_CK_IDENTIFIER 0x64617461 //data

#define IS_PCM 1

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

struct wavCDT {
	RIFF_CK	riff_desc;
	FMT_CK	fmt;
	DATA_CK	data;
};

static wav_t loadWavFromFile(FILE *file);

static int loadRIFF(FILE *file, wav_t wav);
static int loadFMT(FILE *file, wav_t wav);
static int loadDATA(FILE *file, wav_t wav);

static int saveWavToFile(FILE *file, wav_t);
static int saveRIFF(FILE *file, wav_t wav);
static int saveFMT(FILE *file, wav_t wav);
static int saveDATA(FILE *file, wav_t wav);


static int calcRiffSize(FMT_CK fmt, dataHolder_t data);

wav_t newWavFromFile(char *path) {

	FILE *source;
	wav_t ret;

	if((source = fopen(path, "r")) == NULL){
		ERROR("File opening filed\n");
		return NULL;
	}

	ret = loadWavFromFile(source);
	fclose(source);

	return ret;

}

static wav_t loadWavFromFile(FILE *file){

	wav_t wav = NULL;

	if((wav = calloc(sizeof(struct wavCDT),1)) == NULL){ //Everything to 0, so we can check changes later
		ERROR("Alloc failed! The sky will burn!\n");
		return NULL;
	}


	if(!loadRIFF(file, wav) || !loadFMT(file, wav) || !loadDATA(file, wav)){
		if(wav->fmt.extraParams != NULL)
			free(wav->fmt.extraParams);

		if(wav->data.soundData != NULL)
			free(wav->data.soundData);

		free(wav);
		return NULL;
	}

	return wav;

}

static int loadRIFF(FILE *file, wav_t wav) {

	//Reading RIFF chunk
	fread(&wav->riff_desc.chunkID, sizeof(CKID), 1, file);
	wav->riff_desc.chunkID = ntohl(wav->riff_desc.chunkID); //RIFF is saved in big endian
	LOG("Riff CKID: %X\n", wav->riff_desc.chunkID);
	if (wav->riff_desc.chunkID != RIFF_CK_IDENTIFIER){
		WARN("RIFF Header not valid, result: %X\n", wav->riff_desc.chunkID);
		return 0;
	}

	fread(&wav->riff_desc.chunkSize, sizeof(CKSIZE), 1, file);
	LOG("FileSize: %d\n", (int) (wav->riff_desc.chunkSize + sizeof(CKSIZE) + sizeof(DWORD)));

	fread(&wav->riff_desc.format, sizeof(CKSIZE), 1, file);
	wav->riff_desc.format = ntohl(wav->riff_desc.format);

	if(wav->riff_desc.format != RIFF_FORMAT_IDENTIFIER){
		WARN("WAVE Format not valid. Result %X\n", wav->riff_desc.format);
		return 0;
	}
	LOG("File format: %x\n", wav->riff_desc.format);

	return 1;
}


/*
CKID	chunkID;
CKSIZE	chunkSize;
WORD	wFormatTag; // Format category,i.e.:PCM = 1 (no compres.)
WORD	wChannels; // Number of channels:1, mono; 2, stereo
DWORD	dwSamplesPerSec;	// Sampling rate: Mhz DWORD	dwAvgBytesPerSec;
DWORD	dwAvgBytesPerSec;
WORD	wBlockAlign;
WORD	wBitsPerSample; //8, 16, etc.
WORD	extraParamSize;// If PCM, doesn't exist
BYTE	*extraParams;//space for extra params
*/
static int loadFMT(FILE *file, wav_t wav) {

	//Reading fmt chunk
	fread(&wav->fmt.chunkID, sizeof(CKID), 1, file);
	wav->fmt.chunkID = ntohl(wav->fmt.chunkID); //fmt is saved in big endian

	if(wav->fmt.chunkID != FMT_CK_IDENTIFIER){
		WARN("fmt format not valid. Result %X\n",wav->fmt.chunkID);
		return 0;
	}

	LOG("FMT identifier: %X\n",wav->fmt.chunkID);

	fread(&wav->fmt.chunkSize, sizeof(CKSIZE), 1, file);
	LOG("FMT Chunk size %d commonly is 16\n", wav->fmt.chunkSize);

	fread(&wav->fmt.wFormatTag, sizeof(WORD), 1, file);
	LOG("format: %d. For common files PCM = 1\n", wav->fmt.wFormatTag);

	fread(&wav->fmt.wChannels, sizeof(WORD), 1, file);
	LOG("channels: %d. Mono is 1\n", wav->fmt.wChannels);

	fread(&wav->fmt.dwSamplesPerSec, sizeof(DWORD), 1, file);
	LOG("sample rate is: %d. Should be: 8000\n", wav->fmt.dwSamplesPerSec);

	fread(&wav->fmt.dwAvgBytesPerSec, sizeof(DWORD), 1, file);
	LOG("byte rate is: %d\n", wav->fmt.dwAvgBytesPerSec);

	fread(&wav->fmt.wBlockAlign, sizeof(WORD), 1, file);
	LOG("block align: %d\n", wav->fmt.wBlockAlign);

	fread(&wav->fmt.wBitsPerSample, sizeof(WORD), 1, file);
	LOG("bits per sample: %d\n",wav->fmt.wBitsPerSample);

	if(wav->fmt.wFormatTag != IS_PCM){
		fread(&wav->fmt.extraParamSize, sizeof(WORD), 1, file);
		LOG("extraparam size: %d\n", wav->fmt.extraParamSize);
		if ((wav->fmt.extraParams = (BYTE *) malloc(wav->fmt.extraParamSize)) == NULL){
			ERROR("memory alloc failed\n");
			return 0;
		}
		fread(wav->fmt.extraParams, wav->fmt.extraParamSize, 1, file);
	} else {
		LOG("no extra params to load\n");
		wav->fmt.extraParamSize = 0;
		wav->fmt.extraParams = NULL;
	}

	return 1;

}

/*
CKID	chunkID;
CKSIZE	chunkSize;
BYTE	*soundData;
*/
static int loadDATA(FILE *file, wav_t wav) {

	fread(&wav->data.chunkID, sizeof(CKSIZE), 1, file);
	wav->data.chunkID = ntohl(wav->data.chunkID); //Data is in big endian

	if(wav->data.chunkID != DATA_CK_IDENTIFIER){
		WARN("Data identifier don't mach. Result: %x\n", wav->data.chunkID);
		return 0;
	}

	LOG("Data idenfifier is: %x\n", wav->data.chunkID);

	fread(&wav->data.chunkSize, sizeof(CKSIZE), 1, file);
	LOG("Sound data: %d\n", wav->data.chunkSize);

	if((wav->data.soundData = (BYTE *) malloc(wav->data.chunkSize)) == NULL){
		WARN("Memory alloc fail\n");
		return 0;
	}

	fread(wav->data.soundData, wav->data.chunkSize, 1, file);

	return 1;
}


int wavWriteToFile(wav_t wav, char *path) {

	FILE *file = NULL;

	if((file = fopen(path, "w")) == NULL){
		ERROR("File opening failed\n");
		return 0;
	}

	int ret = saveWavToFile(file, wav);
	fclose(file);
	return ret;

}

static int saveWavToFile(FILE *file, wav_t wav) {


	if(!saveRIFF(file, wav) || !saveFMT(file, wav) || !saveDATA(file, wav)){
		ERROR("Saving file failed\n");
		return 0;
	}

	return 1;

}
static int saveRIFF(FILE *file, wav_t wav) {

	CKID aux;

	aux = htonl(wav->riff_desc.chunkID); //RIFF goes in big endiaan
	fwrite(&aux, sizeof(CKID), 1, file); //RIFF

	LOG("writing chunk size. Total file size: %d\n", (int) (wav->riff_desc.chunkSize + sizeof(CKID) + sizeof(CKSIZE)));
	fwrite(&wav->riff_desc.chunkSize, sizeof(CKSIZE), 1, file); //SIZE

	aux = htonl(wav->riff_desc.format); //WAVE goes in big endian
	fwrite(&aux, sizeof(CKID), 1, file); //WAVE

	return 1;
}
static int saveFMT(FILE *file, wav_t wav) {

	CKID aux;

	aux = htonl(wav->fmt.chunkID); // fmt[] goes in big endian
	fwrite(&aux, sizeof(CKID), 1, file); //fmt[]

	fwrite(&wav->fmt.chunkSize, sizeof(CKSIZE), 1, file); //Should be 16 for our files
	fwrite(&wav->fmt.wFormatTag, sizeof(WORD),1, file); //1 for no compresion

	fwrite(&wav->fmt.wChannels, sizeof(WORD), 1, file);
	fwrite(&wav->fmt.dwSamplesPerSec, sizeof(DWORD), 1, file);
	fwrite(&wav->fmt.dwAvgBytesPerSec, sizeof(DWORD), 1, file);

	fwrite(&wav->fmt.wBlockAlign, sizeof(WORD), 1, file);
	fwrite(&wav->fmt.wBitsPerSample, sizeof(WORD), 1, file);

	if(wav->fmt.wFormatTag != IS_PCM){
		LOG("writing extra params of size %d\n",wav->fmt.extraParamSize);
		fwrite(&wav->fmt.extraParamSize, sizeof(WORD), 1, file);
		fwrite(wav->fmt.extraParams, wav->fmt.extraParamSize, 1, file);
	} else {
		LOG("there are not extra params to write\n");
	}

	return 1;

}
static int saveDATA(FILE *file, wav_t wav) {

	CKID aux;

	aux = htonl(wav->data.chunkID); //DATA goes in big endian
	fwrite(&aux, sizeof(CKSIZE), 1, file);
	LOG("writing sound data of %d bytes\n", wav->data.chunkSize);
	fwrite(&wav->data.chunkSize, sizeof(CKSIZE), 1, file);
	fwrite(wav->data.soundData, wav->data.chunkSize, 1, file);

	return 1;

}


wav_t newWavFromData(FMT_CK fmt_ck, dataHolder_t soundData) {

	wav_t wav;

	if((wav = malloc(sizeof(struct wavCDT))) == NULL){
		ERROR("memory allocation failded\n");
		return NULL;
	}
	//Process RIFF
	wav->riff_desc.chunkID = RIFF_CK_IDENTIFIER;
	wav->riff_desc.format = RIFF_FORMAT_IDENTIFIER;
	wav->riff_desc.chunkSize = calcRiffSize(fmt_ck, soundData);


	//Process FMT
	wav->fmt = fmt_ck;
	if(wav->fmt.wFormatTag != IS_PCM){
		LOG("Format is not PCM, allocating extra params. %d\n", wav->fmt.extraParamSize);
		if((wav->fmt.extraParams = malloc(wav->fmt.extraParamSize)) == NULL){
			ERROR("Memory alloc failed\n");
			free(wav);
			return NULL;
		}
	} else {
		LOG("Format is PCM, there is no need extra params\n");
	}


	//Process DATA
	wav->data.chunkID = DATA_CK_IDENTIFIER;
	wav->data.chunkSize = soundData.size;

	LOG("allocating memory for sound data: %d\n", wav->data.chunkSize);
	if((wav->data.soundData = malloc(wav->data.chunkSize)) == NULL){
		ERROR("Memory alloc failed\n");
		if(wav->fmt.extraParamSize != 0)
			free(wav->fmt.extraParams);
		free(wav);
		return NULL;
	}

	memcpy(wav->data.soundData, soundData.data, soundData.size);

	return wav;
}

static int calcRiffSize(FMT_CK fmt, dataHolder_t data) {
	int ret;

	//Space from RIFF_CK
	ret = sizeof(CKID); //format

	//Space from FMT_CK
	ret += sizeof(FMT_CK) - sizeof(BYTE*); //Pointer doesn't go to disk.
	ret += fmt.extraParamSize; //Space of extra params.

	//Space from DATA_CK
	ret += data.size;

	return ret;
}


/**
 * Retorna el tamano de los datos del archivo wav
 */
long wavGetDataSize(wav_t wav) {
	return wav->data.chunkID;
}

/**
 * Retorna una COPIA del vector de datos del archivo wav.
 */
dataHolder_t wavGetData(wav_t wav) {
	dataHolder_t holder = {NULL, 0};

	if((holder.data = malloc(wav->data.chunkSize)) == NULL){
		ERROR("Data alloc failed\n");
		return holder;
	}

	holder.size = wav->data.chunkSize;
	memcpy(holder.data, wav->data.soundData, wav->data.chunkSize);
	return holder;
}

void freeWavFMT(FMT_CK fmt) {
	if(fmt.extraParamSize != 0)
		free(fmt.extraParams);
	return;
}

/**
 * Libera los recursos usados para almacenar el wav.
 */
void freeWav(wav_t wav) {
	DATA_CK data_ck = wav->data;
	FMT_CK fmt_ck = wav->fmt;

	if(fmt_ck.extraParamSize != 0){
		free(fmt_ck.extraParams);
	}

	free(data_ck.soundData);
	freeWavFMT(fmt_ck);
	free(wav);
	return;
}

FMT_CK wavGetFMT(wav_t wav) {

	FMT_CK fmt;

	fmt = wav->fmt;

	if(fmt.extraParamSize != 0){
		LOG("Need space for extra params: %d\n", fmt.extraParamSize);

		if((fmt.extraParams = malloc(fmt.extraParamSize)) == NULL){
			LOG("Memory alloc failed\n");
			fmt.extraParamSize = -1;
		}
	}

	return fmt;
}

