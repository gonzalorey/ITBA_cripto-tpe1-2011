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
#include "debug.h"

struct wavCDT {
	RIFF_CK	riff_desc;
	FMT_CK	fmt;
	DATA_CK	data;
};

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
	if ((wav = malloc(sizeof(struct wavCDT))) == NULL) {
		return NULL;
	}

	wav = wavParser(wav, source);
	if (wav == NULL) {
		LOG("Dio mal!\n");
	} else {
		LOG("Dio Bien!\n");
	}
}

wav_t newWavFromData(FMT_CK* fmt_ck, BYTE* data, long dataSize) {
	RIFF_CK * newRiff_ck = malloc(sizeof(RIFF_CK));
	if (newRiff_ck == NULL) {
		return NULL;
	}
	newRiff_ck->chunkID = RIFF_CKID;
	//newRiff_ck->chunkSize = 		// TODO
	newRiff_ck->format = RIFF_FORMAT;
	FMT_CK * newFmt_ck = malloc(sizeof(FMT_CK));
	if (newFmt_ck == NULL) {
		return NULL;
	}
	memcpy(newFmt_ck, fmt_ck, sizeof(FMT_CK));
	BYTE * extraParams = malloc(sizeof(fmt_ck->extraParams));
	if (newRiff_ck == NULL) {
		return NULL;
	}
	memcpy(extraParams, fmt_ck->extraParams, sizeof(fmt_ck->extraParams));
	newFmt_ck->extraParams = extraParams;
	DATA_CK * newData = malloc(sizeof(DATA_CK));
	if (newData == NULL) {
		return NULL;
	}
	newData->soundData = malloc(dataSize);
	if (newData->soundData == NULL) {
		return NULL;
	}
	memcpy(newData->soundData, data, dataSize);
	newData->chunkID = DATA_CKID;
	newData->chunkSize = dataSize;

	wav_t wav = malloc(sizeof(struct wavCDT));
	if (wav == NULL) {
		return NULL;
	}
	wav->riff_desc = *newRiff_ck;
	wav->fmt = *newFmt_ck;
	wav->data = *newData;

	return wav;
}

/**
 * Guarda un un archivo wav sencillo en el path especificado.
 */
int wavSave(wav_t wav, char*filepath) {
	return 0;
}

/**
 * Retorna el tamano de los datos del archivo wav
 */
long wavGetDataSize(wav_t wav) {
	return sizeof((wav->data).soundData);
}

/**
 * Retorna una COPIA del vector de datos del archivo wav.
 */
void *wavGetData(wav_t wav) {
	void * dataCopy = malloc(sizeof((wav->data).soundData)+1);
	if (dataCopy == NULL) {
		return NULL;
	}
	memcpy(dataCopy, (wav->data).soundData, sizeof((wav->data).soundData));
	return dataCopy;
}

void freeWavFMT(FMT_CK fmt) {
	free(fmt.extraParams);
}

/**
 * Libera los recursos usados para almacenar el wav.
 */
void freeWav(wav_t wav) {
	DATA_CK data_ck = wav->data;
	FMT_CK fmt_ck = wav->fmt;

	free(data_ck.soundData);
	freeWavFMT(fmt_ck);
	free(wav);
}

FMT_CK * wavGetFmt(wav_t wav) {
	BYTE * extraParamsCopy = malloc(sizeof((wav->fmt).extraParams)+1);
	if (extraParamsCopy == NULL) {
		return NULL;
	}
	memcpy(extraParamsCopy, (wav->fmt).extraParams, sizeof((wav->fmt).extraParams));
	FMT_CK * fmt_ckCopy = malloc(sizeof(FMT_CK));
	if (fmt_ckCopy == NULL) {
		return NULL;
	}
	memcpy(fmt_ckCopy, &(wav->fmt), sizeof(wav->fmt));
	fmt_ckCopy->extraParams = extraParamsCopy;
	return fmt_ckCopy;
}

void setFMT (wav_t wav, FMT_CK fmt) {
	wav->fmt = fmt;
}

void setRiff (wav_t wav, RIFF_CK riff) {
	wav->riff_desc = riff;
}

void setData (wav_t wav, DATA_CK data) {
	wav->data = data;
}

FMT_CK getFMT (wav_t wav) {
	return wav->fmt;
}

RIFF_CK getRiff (wav_t wav) {
	return wav->riff_desc;
}

DATA_CK getData (wav_t wav) {
	return wav->data;
}

