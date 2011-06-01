/* main.c */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "main.h"

#include "macros.h"

#include "stegano.h"
#include "fileContainer.h"
#include "wav.h"

#include "typeConverter.h"

#include "crypto.h"

#define DEBUG_LOG
#include "debug.h"

#include "parserADT.h"

int main(int argc, char *argv[]){

	configuration_t *conf = clparser(argc, argv);
	wav_t newCarrier;


	wav_t carrierFile;
	dataHolder_t carrierData;

	showConfig(conf);
	checkConfig(&conf);
	if(conf != NULL) {
		LOG("opening carrier file: %s\n", conf->carrierFile);
		carrierFile = newWavFromFile(conf->carrierFile);
		carrierData = wavGetData(carrierFile);

		if(conf->embed == steg_embed){
			fileContainer_t payload;
			char *ext = NULL;
			dataHolder_t payloadRawHolder, resultPayload;
			FCRead(conf->sourceFile, &payload);

			//Embeding
			if(conf->encriptation == NULL){
				//Embeding without encryptation.
				resultPayload.data = payload.rawData;
				resultPayload.size = payload.size;
				ext = payload.extension;
			} else {
				//Embeding with encryptation.
				typeConvertFCtoDH(&payload, &payloadRawHolder);
				crypto_Execute(*conf->encriptation, payloadRawHolder, &resultPayload);
				ext = NULL;
			}

			stegEmbed(&carrierData, &resultPayload, conf->stegMode, ext);
			newCarrier = newWavFromData(wavGetFMT(carrierFile), carrierData);
			wavWriteToFile(newCarrier, conf->targetFile);

		} else {
			fileContainer_t resultPayloadFile;
			dataHolder_t payloadData;
			if(conf->encriptation == NULL){
				LOG("extracting without decryption\n");
				//Extracting without decription
				stegExtract(&carrierData, &payloadData, conf->stegMode, resultPayloadFile.extension);

				resultPayloadFile.rawData = payloadData.data;
				resultPayloadFile.size = payloadData.size;
				LOG("got payload file size: %d, and extention %s\n", resultPayloadFile.size, resultPayloadFile.extension);
			} else {
				//Extracting with decription
				LOG("extrating with encryption\n");
				dataHolder_t cipherPayload;
				stegExtract(&carrierData, &cipherPayload, conf->stegMode, NULL);
				crypto_Execute(*conf->encriptation, cipherPayload, &payloadData);
				typeConvertDHtoFC(&resultPayloadFile, &payloadData);
			}

			FCWrite(conf->targetFile, &resultPayloadFile);
		}

	}

	return 0;
}


configuration_t *clparser(int argc, char ** argv) {
	configuration_t *conf;
	char *aOpt[] = {"aes128", "aes192", "aes256", "des"};
	char *mOpt[] = {"ecb", "cfb", "ofb", "cbc"};
	char *stegOpt[] = {"LSB1", "LSB4", "LSBE"};


	if((conf = calloc(sizeof(configuration_t),1)) == NULL){
		return NULL;
	}

	parserADT parser = newParser(argc, argv);

	setArg(parser, arg_in, "-in", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, arg_out, "-out", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, arg_porter, "-p", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, arg_embed, "-embed", 0, 1, ARG_TYPE0, 0, NULL);
	setArg(parser, arg_extract, "-extract", 0, 1, ARG_TYPE0, 0, NULL);
	setArg(parser, arg_pass, "-pass", 0, 1, ARG_TYPE1, 0, NULL);
	setArg(parser, arg_algorithm, "-a", 0, 1, ARG_TYPE1, SIZE_OF_ARRAY(aOpt), aOpt);
	setArg(parser, arg_ciphermode, "-m", 0, 1, ARG_TYPE1, SIZE_OF_ARRAY(mOpt) , mOpt);
	setArg(parser, arg_steg, "-steg", 0, 1, ARG_TYPE1, SIZE_OF_ARRAY(stegOpt), stegOpt);
	setArg(parser, arg_help, "-h", 0, 1, ARG_TYPE0, 0, NULL); //help

	int error = 0;
	while(!noMoreArgs(parser) && !error){
		switch(getArg(parser)){
		case arg_out:
			error = !processArg(&conf->targetFile, getValue(parser), "target file");
			break;
		case arg_in:
			error = !processArg(&conf->sourceFile, getValue(parser), "source file");
			break;
		case arg_porter:
			error = !processArg(&conf->carrierFile, getValue(parser), "carrier file");
			break;
		case arg_embed:
			if(conf->embed == steg_notSet){
				LOG("embed/extract is embed\n");
				conf->embed = steg_embed;

				if(conf->encriptation != NULL) {
					conf->encriptation->encrypOrDecrypt = encrypOrDecrypt_encrypt;
				}

			}
			else {
				printf("embed/extract already set\n");
				error = 1;
			}
			break;
		case arg_extract:
			if(conf->embed == steg_notSet){
				LOG("embed/extract is extract\n");
				conf->embed = steg_extract;

				if(conf->encriptation != NULL) {
					conf->encriptation->encrypOrDecrypt = encrypOrDecrypt_decrypt;
				}


			} else {
				printf("embed/extract already set\n");
				error = 1;
			}
			break;
		case arg_pass:
			if(!initEncrypt(conf)){
				error = 1;
			} else {
			error = !processArg((char**)&conf->encriptation->passKeyIv.password, getValue(parser), "password");
			}
			break;
		case arg_algorithm:
			if(!initEncrypt(conf)){
				error = 1;
			} else {
				algorithm_t alg = getCryptoAlgorithm(getValue(parser));
				if(alg != algorithm_none){
					setCryptoAlgorithm(conf->encriptation, alg);
				} else {
					printf("encryptation algorithm not reconized: %s\n", getValue(parser));
					error = 1;
				}
			}
			break;
		case arg_ciphermode:
			if(!initEncrypt(conf)){
				error = 1;
			} else {
				ciphermode_t cipherm = getCipherMode(getValue(parser));
				if(cipherm != ciphermode_none){
					setCryptoCiphermode(conf->encriptation, cipherm);
				} else {
					printf("ciphermode not reconized: %s\n", getValue(parser));
					error = 1;
				}
			}
			break;
		case arg_steg:
			if(conf->stegMode != stegMode_none){
				printf("steg already set\n");
				error = 1;
			} else {
				stegMode_t mode = getStegMode(getValue(parser));
				if(mode == stegMode_none){
					printf("steg mode not reconized: %s\n", getValue(parser));
					error = 1;
				} else {
					conf->stegMode = mode;
				}
			}
			break;
		case arg_help:
			showHelp();
			return NULL;
			break;

		}
	}

	return conf;

}

static int processArg(char **arg, char *value, char *description){
	int lenght = strlen(value);
	LOG("reding %s: \"%s\" of length: %d\n", description, value, lenght);

	if(*arg != NULL){
		WARN("%s already set\n", description);
		return 0;
	}

	if((*arg = malloc(lenght+1)) == NULL){
		ERROR("mem alloc failed\n");
		return 0;
	}

	strcpy(*arg, value);


	return 1;
}

static int initEncrypt(configuration_t *conf) {

	if(conf->encriptation != NULL){
		LOG("initEncrypt already done\n");
		return 1;
	}

	if((conf->encriptation = calloc(sizeof(encryption_t), 1)) == NULL){
		ERROR("initEncrypt memory fail\n");
		return 0;
	}

	setCryptoAlgorithm(conf->encriptation, algorithm_aes128);
	setCryptoCiphermode(conf->encriptation, ciphermode_cbc);

	if(conf->embed != steg_notSet){
		setCryptoEncryptOrDecrypt(conf->encriptation, ((conf->embed == steg_embed)? encrypOrDecrypt_encrypt : encrypOrDecrypt_decrypt));
	}
	return 1;

}

static algorithm_t getCryptoAlgorithm(char *value) {
	LOG("getCryptoAlgorithm with %s\n",value);
	algorithm_t alg = algorithm_none;

	if(strcmp(value, "aes128") == 0) {
		alg = algorithm_aes128;
		LOG("algoritm is aes128\n");
	} else { if(strcmp(value, "aes192") == 0) {
		alg = algorithm_aes192;
		LOG("algoritm is aes192\n");
	} else { if(strcmp(value, "aes256") == 0) {
		alg = algorithm_aes256;
		LOG("algoritm is aes256\n");
	} else { if(strcmp(value, "des") == 0) {
		alg = algorithm_des;
		LOG("algoritm is des\n");
	}else{
		alg = algorithm_none;
		LOG("algoritm is none\n");
	}}}}
	return alg;
}

static ciphermode_t getCipherMode(char *value) {
	ciphermode_t cipherm = ciphermode_none;

	if(strcmp(value, "ecb") == 0) {
		LOG("ciphermode is ecb\n");
		cipherm = ciphermode_ecb;
	} else if(strcmp(value, "cfb") == 0) {
		cipherm = ciphermode_cfb;
		LOG("ciphermode is cfb\n");
	}else if(strcmp(value, "ofb") == 0) {
		cipherm = ciphermode_ofb;
		LOG("ciphermode is ofb\n");
	}else if(strcmp(value, "cbc") == 0) {
		cipherm = ciphermode_cbc;
		LOG("ciphermode is cbc\n");
	}else{
		LOG("ciphermode is none\n");
	}


	return cipherm;
}

static stegMode_t getStegMode(char *value) {
	stegMode_t mode = stegMode_none;

	if(!strcmp(value, "LSB1")){
		LOG("steg mode is lsb1\n");
		mode = stegMode_LSB;
	} else {
		if(!strcmp(value, "LSB4")){
			LOG("steg mode is lsb4\n");
			mode = stegMode_LSB4;
		} else {
			if(!strcmp(value, "LSBE")){
				LOG("steg mode is LSBE\n");
				mode = stegMode_LSBE;
			} else {
				LOG("steg mode is none");
			}
		}
	}

	return mode;

}

static void showHelp(){
	printf("wavStegano 1.0\n");
	printf("Mangiarotti, Nizzo, Rey\n\n");
	printf("Embeding:\n");
	printf("\twavStegano -in <source> -out <target> -p <carrier> -steg <mode> -embed\n\n");
	printf("Extracting:\n");
	printf("\twavStegano -out <target> -p <carrier> -steg <mode> -extract\n\n");
	printf("\t-in : payload file\n");
	printf("\t-out : target file, the carrier with the payload file.\n");
	printf("\t-p : carrier file. The file that will hold the payload\n");
	printf("\t-steg: hide method. LSB1, LSB4, LSBE\n");
	printf("Encriptation:\n");
	printf("Default Algoritm is AES128 and Ciphermode is CBC\n");
	printf("\t-pass specefies the password for use [MANDATORY]\n");
	printf("\t-m specefies the ciphermode. CBC, ECB, OFB, CFB\n");
	printf("\t-a specefies the algorithm. AES128, AES192, AES256, DES\n");

}

static void showConfig(configuration_t *conf) {
	if(conf->embed == steg_notSet){
		printf("what to do not set\n");
	} else {
		printf("we are\n");
		if(conf->embed == steg_extract){
			printf("extracting...\n");
		} else {
			printf("embeding...\n");
		}
	}
	printf("target file: %s\n", conf->sourceFile);
	printf("source file: %s\n", conf->targetFile);
	printf("carrier file: %s\n", conf->carrierFile);
	printf("steg mode: %s\n", getStegModeStr(conf->stegMode));
	if(conf->encriptation == NULL){
		printf("no encryptation\n");
	} else {
		printf("algorithm %s\n", getAlgorithmStr(conf->encriptation->algorithm));
		printf("ciphermode %s\n", getCipherModeStr(conf->encriptation->ciphermode));
		printf("password: %s\n", conf->encriptation->passKeyIv.password);
	}

}

static char *getStegModeStr(stegMode_t mode){
	LOG("getStegModeStr: %d\n", mode);
	switch(mode){
	case stegMode_LSB:
		return "stegMode_lsb1";
	case stegMode_LSB4:
		return "stegMode_lsb4";
	case stegMode_LSBE:
		return "stegMode_lsbe";
	case stegMode_none:
		return "stegMode_none";
	}
	return "stegMode_wtf?";
}

static char *getCipherModeStr(ciphermode_t ciphermode) {
	switch(ciphermode){
	case ciphermode_cbc:
		return "ciphermode_cbc";
	case ciphermode_cfb:
		return "ciphermode_cfb";
	case ciphermode_ecb:
		return "ciphermode_ecb";
	case ciphermode_ofb:
		return "ciphermode_ofb";
	case ciphermode_none:
		return "cipermode_none";
	}
	return "ciphermode_wtf?";
}



static char *getAlgorithmStr(algorithm_t algoritm) {
	switch(algoritm){
	case algorithm_aes128:
		return "algorithm_aes128";
	case algorithm_aes192:
		return "algorithm_aes192";
	case algorithm_aes256:
		return "algorithm_aes256";
	case algorithm_des:
		return "algorithm_des";
	case algorithm_none:
		return "algorithm_none";
	}
	return "algorithm_wtf";
}

static void checkConfig(configuration_t **conf) {
	int error = 0;


	if(*conf == NULL){
		printf("configuration is null\n");
	}

	if((*conf)->embed == steg_notSet){
		printf("operation not set\n");
		*conf = NULL;
		return;
	}

	if((*conf)->stegMode == stegMode_none){
		printf("steg mode not set\n");
		error = 1;
	}

	if((*conf)->carrierFile == NULL){
		printf("carrier file not set\n");
		error = 1;
	}

	if((*conf)->embed == steg_embed) {
		if((*conf)->sourceFile == NULL){
			printf("source file not set\n");
			error = 1;
		}
	}


	if((*conf)->targetFile == NULL){
		printf("target file not set\n");
		error = 1;
	}

	if((*conf)->encriptation != NULL){
		if((*conf)->encriptation->passKeyIv.password == NULL){
			printf("password not set\n");
			error = 1;
		}
	}


	if(error){
		*conf = NULL;
	}

	return;
}

void assertSizes() {
	assert(sizeof(BYTE) == 1);
	assert(sizeof(WORD) == 2);
	assert(sizeof(DWORD) == 4);
}

