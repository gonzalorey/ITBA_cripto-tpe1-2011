/*
 * main.c
 *
 *  Created on: Apr 15, 2011
 */

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "crypto.h"
#include "cryptoTest.h"
#include "types.h"
#include "wav.h"
#include "parserADT.h"
#include "wavParser.h"

//#define RUNCRYPTOTESTS

#define DEBUG_LOG
#include "debug.h"

void assertSizes();

cryptoHolder_t * clparser(int argc, char ** argv);

int main(int argc, char ** argv){
	// Esta variable va a valer 1 se se va a encriptar, 2 si se va a desencriptar.
	// Esto se obtiene a partir del parser, si tiene el -e o el -d
//	int encryptDesencrypt;
	wav_t wav;

	cryptoHolder_t * cryptoHolder;
	if((cryptoHolder = clparser(argc, argv)) == NULL)
		return 0;

	//Probamos que la arquitectura esta configurada correctamente.
	//assertSizes();

#ifdef RUNCRYPTOTESTS
	cryptoRunTests();
#endif

	dataHolder_t target;
	wav = newWavFromFile(cryptoHolder->sourceFile);

	FMT_CK fmt = wavGetFMT(wav);
	dataHolder_t soundData = wavGetData(wav);

	int cryptoResult = crypto_Execute(cryptoHolder->encription, soundData, &target);
	LOG("cryptoResult: %d\n", cryptoResult);

	wav_t wavAux = newWavFromData(fmt,target);

	int result = wavWriteToFile(wavAux, cryptoHolder->targetFile);

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

cryptoHolder_t *
clparser(int argc, char ** argv){
	cryptoHolder_t * ans;

	if((ans = malloc(sizeof(cryptoHolder_t))) == NULL)
		return NULL;

	parserADT parser = newParser(argc, argv);

	char * aOpt[4] = {"aes128", "aes192", "aes256", "des"};
	char * mOpt[4] = {"ecb", "cfb", "ofb", "cbc"};

	setArg(parser, 1, "-in", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, 2, "-out", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, 3, "-e", 0, 1, ARG_TYPE0, 0, NULL);
	setArg(parser, 4, "-d", 0, 1, ARG_TYPE0, 0, NULL);
	setArg(parser, 5, "-pass", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, 6, "-K", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, 7, "-iv", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, 8, "-a", 0, 1, ARG_TYPE1, 4, aOpt);
	setArg(parser, 9, "-m", 0, 1, ARG_TYPE1, 4, mOpt);

	encryption_t encriptation = newEncryptation();

	int error = 0;
	keyIv_t keyIv = {NULL, NULL};
	while(!noMoreArgs(parser) && !error){
		switch (getArg(parser)) {
			case 1:
				printf("Source file: %s\n", getValue(parser));
				if(ans->sourceFile != NULL || (ans->sourceFile = malloc(sizeof(char *) * strlen(getValue(parser)))) == NULL)
					error = 1;

				strcpy(ans->sourceFile, getValue(parser));
				break;

			case 2:
				printf("Target file: %s\n", getValue(parser));
				if(ans->targetFile != NULL || (ans->targetFile = malloc(sizeof(char *) * strlen(getValue(parser)))) == NULL)
					error = 1;

				strcpy(ans->targetFile, getValue(parser));
				break;

			case 3:
				printf("Encription\n");
				if(isSetCryptoEncryptOrDecrypt(encriptation))
					error = 1;
				else
					setCryptoEncryptOrDecrypt(&encriptation, encrypOrDecrypt_encrypt);
				break;

			case 4:
				printf("Desencription\n");
				if(isSetCryptoEncryptOrDecrypt(encriptation))
					error = 1;
				else
					setCryptoEncryptOrDecrypt(&encriptation, encrypOrDecrypt_decrypt);
				break;

			case 5:
				printf("Pass: %s\n", getValue(parser));
				if(isSetCryptoPassKeyIv(encriptation) || isSetCryptoPassOrKey(encriptation)){
					error = 1;
					break;
				}

				// set the encriptation to be with pass
				setCryptoPassOrKey(&encriptation, passOrKey_pass);

				// create the pass and set the encriptation
				passKeyIv_t passKeyIv;
				if((passKeyIv.password = malloc(sizeof(char) * strlen(getValue(parser)))) == NULL){
					error = 1;
					break;
				}

				strcpy(passKeyIv.password, getValue(parser));
				setCryptoPassKeyIv(&encriptation, passKeyIv);
				break;

			case 6:
				printf("Key: %s\n", getValue(parser));
				if(isSetCryptoPassKeyIv(encriptation) || isSetCryptoPassOrKey(encriptation) || keyIv.key != NULL){
					error = 1;
					break;
				}

				if((keyIv.key = malloc(sizeof(char) * strlen(getValue(parser)))) == NULL){
					error = 1;
					break;
				}

				strcpy(keyIv.key, getValue(parser));

				if(keyIv.iv != NULL){
					passKeyIv_t passKeyIv;
					passKeyIv.keyIv = keyIv;
					setCryptoPassKeyIv(&encriptation, passKeyIv);
				}
				break;

			case 7:
				printf("IV: %s\n", getValue(parser));
				if(isSetCryptoPassKeyIv(encriptation) || isSetCryptoPassOrKey(encriptation) || keyIv.iv != NULL){
					error = 1;
					break;
				}

				if((keyIv.iv = malloc(sizeof(char) * strlen(getValue(parser)))) == NULL){
					error = 1;
					break;
				}

				strcpy(keyIv.iv, getValue(parser));

				if(keyIv.key != NULL){
					passKeyIv_t passKeyIv;
					passKeyIv.keyIv = keyIv;
					setCryptoPassKeyIv(&encriptation, passKeyIv);
				}
				break;

			case 8:
				printf("Encription algorithm: %s\n", getValue(parser));
				if(isSetCryptoAlgorithm(encriptation))
					error = 1;
				else{
					algorithm_t alg;
					if(strcmp(getValue(parser), "aes128") == 0)
						alg = algorithm_aes128;
					else if(strcmp(getValue(parser), "aes192") == 0)
						alg = algorithm_aes192;
					else if(strcmp(getValue(parser), "aes256") == 0)
						alg = algorithm_aes256;
					else if(strcmp(getValue(parser), "des") == 0)
						alg = algorithm_des;
					else{
						error = 1;
						break;
					}
					setCryptoAlgorithm(&encriptation, alg);
				}
				break;

			case 9:
				printf("Block method: %s\n", getValue(parser));
				if(isSetCryptoCiphermode(encriptation))
					error = 1;
				else{
					ciphermode_t cipher;
					if(strcmp(getValue(parser), "ecb") == 0)
						cipher = ciphermode_ecb;
					else if(strcmp(getValue(parser), "cfb") == 0)
						cipher = ciphermode_cfb;
					else if(strcmp(getValue(parser), "ofb") == 0)
						cipher = ciphermode_ofb;
					else if(strcmp(getValue(parser), "cbc") == 0)
						cipher = ciphermode_cbc;
					else{
						error = 1;
						break;
					}
					setCryptoAlgorithm(&encriptation, cipher);
				}
				break;

			default:
				error = 1;
				break;
		}
	}

	if(error){
		printf("ERROR\n");
		return NULL;
	}

	freeParser(parser);

	ans->encription = encriptation;

	return ans;
}
