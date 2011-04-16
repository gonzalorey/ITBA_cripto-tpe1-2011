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

int main(void){
	//Probamos que la arquitectura esta configurada correctamente.
	assertSizes();


	newWavFromPath("8k16bitpcm.wav");


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
