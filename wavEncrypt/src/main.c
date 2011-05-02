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

#define DEBUG_WARN
#include "debug.h"

void assertSizes();
void showHelp();

cryptoHolder_t * clparser(int argc, char ** argv);

int checkCryptoHolder(cryptoHolder_t *holder);

int main(int argc, char ** argv){
	wav_t wav;
	int err = 0;

	//Probamos que la arquitectura esta configurada correctamente.
	assertSizes();

	#ifdef RUNCRYPTOTESTS
		cryptoRunTests();
	#endif

	cryptoHolder_t * cryptoHolder;


	if((cryptoHolder = clparser(argc, argv)) == NULL || !checkCryptoHolder(cryptoHolder)) {
		return 0;
	}

	dataHolder_t target;
	wav = newWavFromFile(cryptoHolder->sourceFile);
	if (wav == NULL)
		return 1;

	FMT_CK fmt = wavGetFMT(wav);
	dataHolder_t soundData = wavGetData(wav);

	int cryptoResult = 0;
	if (!err) {
	 cryptoResult = crypto_Execute(cryptoHolder->encription, soundData, &target);
	}
	LOG("cryptoResult: %d\n", cryptoResult);
	int result = 0;
	if (cryptoResult != 0) {

		wav_t wavAux = newWavFromData(fmt,target);

		result = wavWriteToFile(wavAux, cryptoHolder->targetFile);

		LOG("Save result: %s\n", (result)? "Success":"Fail");
		freeWav(wavAux);
		return !result;
	}

	freeWavFMT(fmt);
	dataHolderFree(soundData);
	freeWav(wav);

	LOG("Ended\n");
	return result;
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

	if((ans = calloc(sizeof(cryptoHolder_t), 1)) == NULL)
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
	setArg(parser, 10, "-h", 0, 1, ARG_TYPE0, 0, NULL); //help

	encryption_t encriptation = newEncryptation();

	int error = 0;
	keyIv_t keyIv = {NULL, NULL};
	passOrKey_t passOrKey = -1;
	while(!noMoreArgs(parser) && !error){
		switch (getArg(parser)) {
			case 1:
				LOG("reading source file\n");
				printf("Source file: %s\n", getValue(parser));

				if(ans->sourceFile != NULL){
					WARN("sourceFile already set\n");
					error = 1;
					break;
				}

				if((ans->sourceFile = malloc(sizeof(char) * (strlen(getValue(parser)) + 1))) == NULL) {
					ERROR("mem alloc failed\n");
					error = 1;
					break;
				}

				strcpy(ans->sourceFile, getValue(parser));
				LOG("finish reading source file\n");
				break;
			case 2:
				LOG("reading target  file\n");
				printf("Target file: %s\n", getValue(parser));

				if(ans->targetFile != NULL){
					WARN("targetFile already set\n");
					error = 1;
					break;
				}

				if((ans->targetFile = malloc(sizeof(char) * (strlen(getValue(parser)) + 1))) == NULL) {
					ERROR("mem alloc failed\n");
					error = 1;
					break;
				}
				strcpy(ans->targetFile, getValue(parser));
				LOG("finish reading target file\n");
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

				// create the pass and set the encriptation
				passKeyIv_t passKeyIv;
				if((passKeyIv.password = malloc(sizeof(char) * (strlen(getValue(parser)) + 1))) == NULL){
					ERROR("memory alloc failed\n");
					error = 1;
					break;
				}

				passOrKey = passOrKey_pass;
				setCryptoPassOrKey(&encriptation, passOrKey);
				strcpy((char*)passKeyIv.password, getValue(parser));
				setCryptoPassKeyIv(&encriptation, passKeyIv);
				break;

			case 6:
				printf("Key: %s\n", getValue(parser));
				if(keyIv.key != NULL){
					error = 1;
					break;
				}

				if((keyIv.key = malloc(sizeof(char) * strlen(getValue(parser)) + 1)) == NULL){
					error = 1;
					break;
				}

				strcpy((char*)keyIv.key, getValue(parser));

				if(keyIv.iv != NULL){
					passKeyIv_t passKeyIv;
					passKeyIv.keyIv = keyIv;
					setCryptoPassKeyIv(&encriptation, passKeyIv);
				}
				passOrKey = passOrKey_key;
				setCryptoPassOrKey(&encriptation, passOrKey);
				break;

			case 7:
				printf("IV: %s\n", getValue(parser));
				if(keyIv.iv != NULL){
					error = 1;
					break;
				}

				if((keyIv.iv = malloc(sizeof(char) * strlen(getValue(parser)) + 1)) == NULL){
					error = 1;
					break;
				}

				strcpy((char*)keyIv.iv, getValue(parser));

				if(keyIv.key != NULL){
					passKeyIv_t passKeyIv;
					passKeyIv.keyIv = keyIv;
					setCryptoPassKeyIv(&encriptation, passKeyIv);
				}
				break;
				passOrKey = passOrKey_key;
				setCryptoPassOrKey(&encriptation, passOrKey);
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
					if(strcmp(getValue(parser), "ecb") == 0) {
						LOG("ciphermode is ecb\n");
						cipher = ciphermode_ecb;
					} else if(strcmp(getValue(parser), "cfb") == 0) {
						cipher = ciphermode_cfb;
						LOG("ciphermode is cfb\n");
					}else if(strcmp(getValue(parser), "ofb") == 0) {
						cipher = ciphermode_ofb;
						LOG("ciphermode is ofb\n");
					}else if(strcmp(getValue(parser), "cbc") == 0) {
						cipher = ciphermode_cbc;
						LOG("ciphermode is cbc\n");
					}else{
						error = 1;
						break;
					}
					setCryptoCiphermode(&encriptation, cipher);
				}
				break;

			case 10:
				showHelp();
				return NULL;
				break;

			default:
				error = 1;
				break;
		}
	}


	if(error){
		printf("ERROR: Argument not understood %s\n", getValue(parser));
		return NULL;
	}

	freeParser(parser);

	ans->encription = encriptation;

	return ans;
}


void showHelp() {
	printf("Help:\n");
	printf("\tArguments:\n");
	printf("\t\t-h shows this help\n");
	printf("\t\t-in <wav file> the file source\n");
	printf("\t\t-out <wav file> file file target\n");
	printf("\t\t-d for decrypt\n");
	printf("\t\t-e for encrypt\n");
	printf("\t\t-pass <password> for specificating the password for encrypt/decrypt\n");
	printf("\t\t-K <key> for specifification the key for the algorithm. Usage with -iv\n");
	printf("\t\t-iv <initVect> fror specification the initialition vector. Usage with -K\n");
	printf("\t\t-a <aes128 | aes192 | aes256 | des> the algorithm for use in the encriptation/decryptation\n");
	printf("\t\t-m <ecb | cfb | ofb | cbc> for specification of the cipher mode\n");
	return;
}


int checkCryptoHolder(cryptoHolder_t *holder) {

	char *seeHelp = "See the help with -h";

	if(holder->sourceFile == NULL || (strcmp(holder->sourceFile, "") == 0)){
		printf("sourceFile not set. %s\n", seeHelp);
		return 0;
	}

	if(holder->targetFile == NULL || (strcmp(holder->targetFile, "") == 0)){
		printf("targetFile not set. %s\n", seeHelp);
		return 0;
	}


	if(!isSetCryptoAlgorithm(holder->encription)) {
		printf("algorithm not set. %s\n", seeHelp);
		return 0;
	}

	if(!isSetCryptoCiphermode(holder->encription)){
		printf("ciphermode not set. %s\n", seeHelp);
		return 0;
	}

	if(!isSetCryptoEncryptOrDecrypt(holder->encription)){
		printf("encriptation/decription operation not set. %s\n", seeHelp);
		return 0;
	}

	if(!isSetCryptoPassOrKey(holder->encription)){
		printf("pass or key/iv usage not set.%s\n", seeHelp);
		return 0;
	}

	if(holder->encription.passOrKey == passOrKey_key){
		if(holder->encription.passKeyIv.keyIv.key == NULL || strcmp((char*)holder->encription.passKeyIv.keyIv.key, "") == 0){
			printf("key not set. %s\n", seeHelp);
			return 0;
		}
		if(holder->encription.passKeyIv.keyIv.iv == NULL || strcmp((char*)holder->encription.passKeyIv.keyIv.iv, "") == 0){
			printf("iv not set. %s\n", seeHelp);
			return 0;
		}
	}


	if(holder->encription.passOrKey == passOrKey_pass){
		if(holder->encription.passKeyIv.password == NULL || strcmp((char*)holder->encription.passKeyIv.password, "") == 0){
			printf("password not set. %s\n", seeHelp);
			return 0;
		}
	}


	return 1;
}
