/*
 * wavParser.c
 *
 *  Created on: Apr 16, 2011
 */

#include <stdio.h>
#include <arpa/inet.h>
#include "wav.h"
#include "debug.h"

int parseRiff (CKID	chunkID, CKSIZE	chunkSize, CKID	format);
int parseFMK (CKID	chunkID, CKSIZE	chunkSize, WORD	wFormatTag, WORD wChannels, WORD wBitsPerSample);
int parseData (CKID	chunkID);

wav_t wavParser (wav_t wav, FILE * source) {
	RIFF_CK riff;
	FMT_CK fmt;
	DATA_CK data;
	int bytes;

	if ((bytes = fread(&riff, sizeof(RIFF_CK), 1, source)) == 0){
		FATAL("Cantidad de bytes leidos: %d\n", bytes);
		return 0;
	}
	if (!parseRiff(riff.chunkID, riff.chunkSize, riff.format)) {
		return 0;
	}
	if ((bytes = fread(&fmt, sizeof(FMT_CK), 1, source)) == 0){
		FATAL("Cantidad de bytes leidos: %d\n", bytes);
		return 0;
	}
	if (!parseFMK(fmt.chunkID, fmt.chunkSize, fmt.wFormatTag, fmt.wChannels, fmt.wBitsPerSample)) {
		return 0;
	}
	if (fmt.wFormatTag == FMT_FORMAT_CAT) {
		fseek(source, -8, SEEK_CUR);
	}
	if ((bytes = fread(&data, sizeof(CKID) + sizeof(CKSIZE), 1, source)) == 0) {
		FATAL("Cantidad de bytes leidos: %d\n", bytes);
		return 0;
	}
	data.chunkID = ntohl(data.chunkID);
	if ((data.soundData = malloc(data.chunkSize*4)) == NULL) {
		return 0;
	}
	if ((bytes = fread(data.soundData, data.chunkSize, 1, source)) == 0) {
		FATAL("Cantidad de bytes leidos: %d\n", bytes);
		return 0;
	}
	if (!parseData(data.chunkID)) {
		return 0;
	}
	setData(wav, data);
	setFMT(wav, fmt);
	setRiff(wav, riff);
	return wav;
}

int parseRiff (CKID	chunkID, CKSIZE	chunkSize, CKID	format) {
	chunkID = ntohl(chunkID);
	chunkSize = ntohl(chunkSize);
	format = ntohl(format);
	if (chunkID != RIFF_CKID || format != RIFF_FORMAT) {
		return 0;
	}
	//TODO
//	if (chunkSize != ?????) {
//		return 0;
//	}
	return 1;
}

int parseFMK (CKID	chunkID, CKSIZE	chunkSize, WORD	wFormatTag, WORD wChannels, WORD wBitsPerSample) {
	chunkID = ntohl(chunkID);
	if (chunkID != FMT_CKID || chunkSize != FMT_CKSIZE || wFormatTag != FMT_FORMAT_CAT ||
			wChannels != CHANNEL_MONO) {
		return 0;
	}
	if (wBitsPerSample % 8 != 0) {
		return 0;
	}
	return 1;
}

int parseData (CKID	chunkID) {
	if (chunkID != DATA_CKID) {
		return 0;
	}
	return 1;
}
