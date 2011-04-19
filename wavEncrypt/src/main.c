/*
 * main.c
 *
 *  Created on: Apr 15, 2011
 */

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include "debug.h"
#include "types.h"
#include "wav.h"

void assertSizes();

int main(int argv, char ** argc){
	// Esta variable va a valer 1 se se va a encriptar, 2 si se va a desencriptar.
	// Esto se obtiene a partir del parser, si tiene el -e o el -d
//	int encryptDesencrypt;
	wav_t wav;

	//Probamos que la arquitectura esta configurada correctamente.
	assertSizes();

	// -----------------------------------------------------------------------------
	// Aca se invoca al parser del colo
	// -----------------------------------------------------------------------------

	wav = newWavFromPath(argc[1]);


	return 0;

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
