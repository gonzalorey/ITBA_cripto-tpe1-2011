/*
 * wav.h
 *
 *  Created on: Apr 15, 2011
 */

#ifndef WAV_H_
#define WAV_H_

#include "types.h"

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
	CKID	chunkID;
	CKSIZE	chunkSize;
	CKID	format;
} RIFF_CK;

typedef struct {
	CKID	chunkID;
	CKSIZE	chunkSize;
	BYTE	*soundData;
} DATA_CK;

typedef struct wavCDT *wav_t;

/**
 * Crea un archivo wav sencillo a partir de un path.
 */
wav_t newWavFromPath(char *filepath);

/**
 * Crea un archivo wav sencillo.
 */
wav_t newWavFromData(FMT_CK* fmt_ck, BYTE* data, long dataSize);

/**
 * Guarda un un archivo wav sencillo en el path especificado.
 */
int wavSave(wav_t wav, char*filepath);

/**
 * Retorna el tamano de los datos del archivo wav
 */
long wavGetDataSize(wav_t wav);

/**
 * Retorna una COPIA del vector de datos del archivo wav.
 */
void *wavGetData(wav_t wav);

/**
 * Retorna un puntero a una COPIA del fmt_ck del archivo wav
 */
FMT_CK * wavGetFMT (wav_t wav);

/**
 * Libera los recursos usados para almacenar el wav.
 */
void freeWav(wav_t wav);

/**
 * Libera los recursos usados para almacenar el fmt_ck
 */
void freeWavFMT(FMT_CK fmt);

void setFMT (wav_t wav, FMT_CK fmt);

void setRiff (wav_t wav, RIFF_CK riff);

void setData (wav_t wav, DATA_CK data);

FMT_CK getFMT (wav_t wav);

RIFF_CK getRiff (wav_t wav);

DATA_CK getData (wav_t wav);


#endif /* WAV_H_ */
