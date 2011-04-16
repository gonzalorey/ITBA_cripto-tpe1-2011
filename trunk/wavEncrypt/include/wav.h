/*
 * wav.h
 *
 *  Created on: Apr 15, 2011
 */

#ifndef WAV_H_
#define WAV_H_


typedef struct wavCDT *wav_t;

/**
 * Crea un archivo wav sencillo a partir de un path.
 */
wav_t newWavFromPath(char *filepath);

/**
 * Crea un archivo wav sencillo a partir de un vector de datos.
 */
wav_t newWavFromData(void* data, long size);

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
 * Libera los recursos usados para almacenar el wav.
 */
void freeWav(wav_t wav);


#endif /* WAV_H_ */
