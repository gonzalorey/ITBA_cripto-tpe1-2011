/*
 * main.c
 *
 *  Created on: Apr 15, 2011
 */

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include "cryptoTest.h"
#include "types.h"
#include "wav.h"

//#define RUNCRYPTOTESTS


#define DEBUG_LOG
#include "debug.h"

void assertSizes();

int main(int argv, char ** argc){
	// Esta variable va a valer 1 se se va a encriptar, 2 si se va a desencriptar.
	// Esto se obtiene a partir del parser, si tiene el -e o el -d
//	int encryptDesencrypt;
	wav_t wav;

	//Probamos que la arquitectura esta configurada correctamente.
	assertSizes();

#ifdef RUNCRYPTOTESTS
	cryptoRunTests();
#endif


	// -----------------------------------------------------------------------------
	// Aca se invoca al parser del colo
	// -----------------------------------------------------------------------------

	wav = newWavFromFile("wav/la-mi.wav");

	FMT_CK fmt = wavGetFMT(wav);
	dataHolder_t soundData = wavGetData(wav);

	wav_t wavAux = newWavFromData(fmt,soundData);

	int result = wavWriteToFile(wavAux, "la-miAUX.wav");

	LOG("Save result: %s\n", (result)? "Success":"Fail");

	freeWavFMT(fmt);
	dataHolderFree(soundData);
	freeWav(wav);
	freeWav(wavAux);

	LOG("Ended");
	return !result;

}

/**
 * Checkea que los tamanos de los tipos definidos sean realmente los que
 * deberian ser. Esto es para asegurarse la lectura correcta del archivo
 * en distintas arquitecturas.
 */
void assertSizes() {
	assert(sizeof(BYTE) == 1);
	assert(sizeof(WORD) == 2);
	assert(sizeof(DWORD) == 4);
}
