/* main.c */
#include <stdio.h>
#include <string.h>

#include "stegano.h"
#include "fileContainer.h"
#include "wav.h"

#define DEBUG_LOG
#include "debug.h"


int main(int argc, char *argv[]){
	fileContainer_t payload;
	dataHolder_t payloadData;
	dataHolder_t carrierData;
	wav_t wavCarrier;
	int extract;

	extract = strcmp(argv[4], "ext") == 0;

	wavCarrier = newWavFromFile(argv[1]);
	carrierData = wavGetData(wavCarrier);


	if(extract) {
		LOG("extract\n");
		fileContainer_t payloadFile;
		stegExtract(&carrierData, &payloadData, stegMode_LSB, payloadFile.extension);
		payloadFile.rawData = payloadData.data;
		payloadFile.size = payloadData.size;
		FCWrite(argv[2], &payloadFile);
	} else {
		LOG("embed\n");
		FCRead(argv[2], &payload);
		LOG("payload.size = %d; payload.extention = %s\n", payload.size, payload.extension);
		payloadData = newDataHolder((BYTE*) payload.rawData, payload.size);
		stegEmbed(&carrierData, &payloadData, stegMode_LSB, payload.extension);
		wav_t newCarrier = newWavFromData(wavGetFMT(wavCarrier), carrierData);
		wavWriteToFile(newCarrier, argv[3]);
	}






	return 0;
}
