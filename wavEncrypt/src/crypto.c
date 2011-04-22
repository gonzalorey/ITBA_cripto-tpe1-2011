/* crypto.c */

#include "crypto.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/md5.h>


#define EXTRA_SPACE 1024 //Extra space for algorithm... who knows?

typedef int (*fncInit_t)(EVP_CIPHER_CTX*, const EVP_CIPHER*, const unsigned char *, const unsigned char *);
typedef int (*fncUpdate_t)(EVP_CIPHER_CTX*, unsigned char *, int *, const unsigned char*, int);
typedef int (*fncFinal_t)(EVP_CIPHER_CTX*, unsigned char *,int *);

static void genKey(unsigned char *password,unsigned char *key);
static void genIv(unsigned char *password,unsigned char *iv);

static const  EVP_CIPHER *getChiper(algorithm_t algorithm, ciphermode_t cipher);

encryption_t newEncryptation(){
	encryption_t enc;

	enc.algorithm = -1;
	enc.ciphermode = -1;
	enc.encrypOrDecrypt = -1;
	enc.passKeyIv.password = NULL;
	enc.passKeyIv.keyIv.iv = NULL;
	enc.passKeyIv.keyIv.key = NULL;
	enc.passOrKey = -1;

	return enc;
}

int setCryptoAlgorithm(encryption_t * enc, algorithm_t algorithm){
	if(enc->algorithm != -1)
		return -1;

	enc->algorithm = algorithm;

	return 0;
}

int setCryptoCiphermode(encryption_t * enc, ciphermode_t ciphermode){
	if(enc->ciphermode != -1)
		return -1;

	enc->ciphermode;

	return 0;
}

int setCryptoEncryptOrDecrypt(encryption_t * enc, encrypOrDecrypt_t encryptOrDecrypt){
	if(enc->encrypOrDecrypt != -1)
		return -1;

	enc->encrypOrDecrypt = encryptOrDecrypt;

	return 0;
}

int setCryptoPassKeyIv(encryption_t * enc, passKeyIv_t passKeyIv){
	if()
		return -1;

	enc->passKeyIv = passKeyIv;

	return 0;
}

int setCryptoPassOrKey(encryption_t * enc, passOrKey_t passOrKey){
	if()
		return -1;

	enc->passOrKey = passOrKey;

	return 0;
}

int crypto_Execute(encryption_t encryptation, dataHolder_t source,
		dataHolder_t *target) {

	//Key and IV holders
	unsigned char key[EVP_MAX_KEY_LENGTH] = {0};
	unsigned char iv[EVP_MAX_IV_LENGTH] = {0};

	//Context holder
	EVP_CIPHER_CTX ctx;

	//Encrypt / Decrypt functions
	fncInit_t fncInit;
	fncUpdate_t fncUpdate;
	fncFinal_t fncFinal;

	long allocatedSize; //The data size of memory allocated;

	//Getting enogth space for result
	if((target->data = malloc(source.size + EXTRA_SPACE)) == NULL){
		ERROR("Memory Allocation failed\n");
		return 0;
	}
	target->size = source.size + EXTRA_SPACE;
	allocatedSize = target->size;
	LOG("Memory at: %p\n", target->data);


	//Get the key and the iv
	if (encryptation.passOrKey == passOrKey_key){
		LOG("Copying key and iv from structure\n");
		memcpy(key, encryptation.passKeyIv.keyIv.key, MD5_DIGEST_LENGTH);
		memcpy(iv, encryptation.passKeyIv.keyIv.iv, MD5_DIGEST_LENGTH);
	} else {
		LOG("Generating key and iv from password\n");
		genKey(encryptation.passKeyIv.password, key);
		genIv(encryptation.passKeyIv.password, iv);
	}

	//Choose what to do... encrypt or decrypt?
	if(encryptation.encrypOrDecrypt == encrypOrDecrypt_encrypt){
		LOG("Encryptation\n");
		fncInit = EVP_EncryptInit;
		fncUpdate = EVP_EncryptUpdate;
		fncFinal = EVP_EncryptFinal;
	} else {
		LOG("Dencryptation\n");
		fncInit = EVP_DecryptInit;
		fncUpdate = EVP_DecryptUpdate;
		fncFinal = EVP_DecryptFinal;
	}

	//Do what we came for
	LOG("Init process\n");
	fncInit(&ctx, getChiper(encryptation.algorithm, encryptation.ciphermode), key, iv);
	LOG("Update process\n");
	fncUpdate(&ctx, (unsigned char *) target->data, &target->size, source.data, source.size);
	LOG("Final process targetSize: %d\n", target->size);
	LOG("Target offset for final: %p\n", target->data + target->size);
	int extra; //How much extra of info we are using.
	fncFinal(&ctx,target->data + target->size, &extra);
	LOG("Extra size: %d\n", extra);

	target->size += extra;
	LOG("Final total size: %d\n", target->size);
	LOG("Encryptation/Dencryptation ended.\n");
	//Redimention of target data, so we don't waste memory.
	if(allocatedSize > target->size){
		LOG("Freeing spared memory to %d\n", target->size);
		target->data = realloc(target->data, target->size);
		//We are "freeing memory", so pointer should not move memory.
		//This should not fail.
	}

	return 1;
}

static void genKey(unsigned char *password,unsigned char *key){
	MD5(password, strlen((char *)password), key);
}

static void genIv(unsigned char *password,unsigned char *iv){
	genKey(password, iv);
	genKey(iv, iv); //Le aplico MD5 a la clave para sacar el iv... ?
	//TODO: preguntar si esto est� bien.
}

static const EVP_CIPHER *getChiper(algorithm_t algorithm, ciphermode_t cipher) {

	if (algorithm == algorithm_des) {
		switch (cipher) {
		case ciphermode_cbc:
			return EVP_des_cbc();
			break;
		case ciphermode_cfb:
			return EVP_des_cfb64(); //Es la que usa DES por defaults
			break;
		case ciphermode_ecb:
			return EVP_des_ecb();
			break;
		case ciphermode_ofb:
			return EVP_des_ofb();
			break;
		}
	} else {
		switch(cipher){
		case ciphermode_cbc: case ciphermode_cfb:
		case ciphermode_ecb: case ciphermode_ofb:
			//TODO: Hacer el switch de AES
			return EVP_enc_null();
			break;
		}
	}
	return EVP_enc_null(); //NO hacer nada
}
