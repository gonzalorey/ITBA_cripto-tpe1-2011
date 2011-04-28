/*
 * main.c
 *
 *  Created on: Apr 15, 2011
 */

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include "crypto.h"
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


	dataHolder_t target;
	wav = newWavFromFile("wav/fun2AESCBC.wav");

	encryption_t enc = newEncryptation();
	passKeyIv_t passKeyIv;

	setCryptoAlgorithm(&enc, algorithm_aes128);
	setCryptoCiphermode(&enc, ciphermode_cbc);
	setCryptoEncryptOrDecrypt(&enc, encrypOrDecrypt_decrypt);
	setCryptoPassOrKey(&enc, passOrKey_pass);
	passKeyIv.password = (unsigned char*)"sorpresa";
	setCryptoPassKeyIv(&enc, passKeyIv);

	FMT_CK fmt = wavGetFMT(wav);
	dataHolder_t soundData = wavGetData(wav);

	int cryptoResult = crypto_Execute(enc, soundData, &target);
	LOG("cryptoResult: %d\n", cryptoResult);

	wav_t wavAux = newWavFromData(fmt,target);

	int result = wavWriteToFile(wavAux, "sorpresa.wav");

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
