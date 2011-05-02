/* crypto.c */

#include "crypto.h"
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "macros.h"

#define DEBUG_WARN
#include "debug.h"

#define EMPTY -1

#define EXTRA_SPACE 1024 //Extra space for algorithm... who knows?
typedef int (*fncInit_t)(EVP_CIPHER_CTX*, const EVP_CIPHER*,
		const unsigned char *, const unsigned char *);
typedef int (*fncUpdate_t)(EVP_CIPHER_CTX*, unsigned char *, int *,
		const unsigned char*, int);
typedef int (*fncFinal_t)(EVP_CIPHER_CTX*, unsigned char *, int *);

static const EVP_CIPHER *getChiper(algorithm_t algorithm, ciphermode_t cipher);

typedef const EVP_CIPHER* (*fncCipher_t)(void);

// Mode          cbc    cfb64   ecb    ofb
// algoritmh
// des
// aes128
// aes192
// aes256

const static fncCipher_t ciphers[4][4] =
		{ { EVP_des_cbc, EVP_des_cfb8, EVP_des_ecb, EVP_des_ofb },
		{	EVP_aes_128_cbc, EVP_aes_128_cfb8, EVP_aes_128_ecb, EVP_aes_128_ofb },
		{   EVP_aes_192_cbc, EVP_aes_192_cfb8, EVP_aes_192_ecb, EVP_aes_192_ofb },
		{   EVP_aes_256_cbc, EVP_aes_256_cfb8, EVP_aes_256_ecb, EVP_aes_256_ofb } };

encryption_t newEncryptation() {
	encryption_t enc;

	enc.algorithm = EMPTY;
	enc.ciphermode = EMPTY;
	enc.encrypOrDecrypt = EMPTY;
	enc.passKeyIv.password = NULL;
	enc.passKeyIv.keyIv.iv = NULL;
	enc.passKeyIv.keyIv.key = NULL;
	enc.passOrKey = EMPTY;

	return enc;
}

int isSetCryptoAlgorithm(encryption_t enc) {
	return enc.algorithm != EMPTY;
}

int isSetCryptoCiphermode(encryption_t enc) {
	return enc.ciphermode != EMPTY;
}

int isSetCryptoEncryptOrDecrypt(encryption_t enc) {
	return enc.encrypOrDecrypt != EMPTY;
}

int isSetCryptoPassKeyIv(encryption_t enc) {
	if (!isSetCryptoPassOrKey(enc)) {
		return 0;
	}

	if (enc.passOrKey == passOrKey_key)
		return enc.passKeyIv.keyIv.iv != NULL && enc.passKeyIv.keyIv.key
				!= NULL;
	else
		return enc.passKeyIv.password != NULL;
}

int isSetCryptoPassOrKey(encryption_t enc) {
	return enc.passOrKey != EMPTY;
}

int setCryptoAlgorithm(encryption_t * enc, algorithm_t algorithm) {
	enc->algorithm = algorithm;
	return 1;
}

int setCryptoCiphermode(encryption_t * enc, ciphermode_t ciphermode) {
	enc->ciphermode = ciphermode;
	return 1;
}

int setCryptoEncryptOrDecrypt(encryption_t * enc,
		encrypOrDecrypt_t encryptOrDecrypt) {
	enc->encrypOrDecrypt = encryptOrDecrypt;
	return 1;
}

int setCryptoPassKeyIv(encryption_t * enc, passKeyIv_t passKeyIv) {

	if (!isSetCryptoPassOrKey(*enc))
		return 0;

	if (enc->passOrKey == passOrKey_key) {
		if (passKeyIv.keyIv.iv == NULL || passKeyIv.keyIv.key == NULL) {
			return 0;
		}
	} else {
		if (passKeyIv.password == NULL) {
			return 0;
		}
	}

	enc->passKeyIv = passKeyIv;
	return 1;
}

int setCryptoPassOrKey(encryption_t * enc, passOrKey_t passOrKey) {
	enc->passOrKey = passOrKey;
	return 0;
}

int crypto_Execute(encryption_t encryptation, dataHolder_t source,
		dataHolder_t *target) {

	if (!isCryptoValid(encryptation)){
		WARN("encryptation is not valid\n");
		return 0;
	}

	//Key and IV holders
	unsigned char key[EVP_MAX_KEY_LENGTH] = { 0 };
	unsigned char iv[EVP_MAX_IV_LENGTH] = { 0 };

	//Context holder
	EVP_CIPHER_CTX ctx;

	//Encrypt / Decrypt functions
	fncInit_t fncInit;
	fncUpdate_t fncUpdate;
	fncFinal_t fncFinal;

	long allocatedSize; //The data size of memory allocated;

	//Getting enogth space for result
	if ((target->data = malloc(source.size + EXTRA_SPACE)) == NULL) {
		ERROR("Memory Allocation failed\n");
		return 0;
	}
	target->size = source.size + EXTRA_SPACE;
	allocatedSize = target->size;
	LOG("Memory at: %p\n", target->data);

	//Get the key and the iv
	if (encryptation.passOrKey == passOrKey_key) {
		LOG("Copying key and iv from structure\n");
		int keylen = strlen((char*)encryptation.passKeyIv.keyIv.key);
		int ivlen = strlen((char*)encryptation.passKeyIv.keyIv.iv);
		memcpy(key, encryptation.passKeyIv.keyIv.key, LOWER(EVP_MAX_KEY_LENGTH, keylen));
		memcpy(iv, encryptation.passKeyIv.keyIv.iv, LOWER(EVP_MAX_IV_LENGTH, ivlen));
	} else {
		LOG("Generating key and iv from password\n");
		EVP_BytesToKey(getChiper(encryptation.algorithm, encryptation.ciphermode), EVP_md5(), NULL, encryptation.passKeyIv.password, strlen((char*)encryptation.passKeyIv.password),1, key, iv);
	}

	//Choose what to do... encrypt or decrypt?
	if (encryptation.encrypOrDecrypt == encrypOrDecrypt_encrypt) {
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
	fncInit(&ctx, getChiper(encryptation.algorithm, encryptation.ciphermode),
			key, iv);
	LOG("Update process\n");
	fncUpdate(&ctx, (unsigned char *) target->data, &target->size, source.data,
			source.size);
	LOG("Final process targetSize: %d\n", target->size);
	LOG("Target offset for final: %p\n", target->data + target->size);
	int extra; //How much extra of info we are using.
	fncFinal(&ctx, target->data + target->size, &extra);
	LOG("Extra size: %d\n", extra);

	target->size += extra;
	LOG("Final total size: %d\n", target->size);
	LOG("Encryptation/Dencryptation ended.\n");
	//Redimention of target data, so we don't waste memory.
	if (allocatedSize > target->size) {
		LOG("Freeing spared memory to %d\n", target->size);
		target->data = realloc(target->data, target->size);
		//We are "freeing memory", so pointer should not move memory.
		//This should not fail.
	}

	EVP_CIPHER_CTX_cleanup(&ctx);

	return 1;
}

int isCryptoValid(encryption_t enc) {
	return isSetCryptoAlgorithm(enc) && isSetCryptoCiphermode(enc)
			&& isSetCryptoEncryptOrDecrypt(enc) && isSetCryptoPassOrKey(enc)
			&& isSetCryptoPassKeyIv(enc);
}

void cryptoShowEnc(encryption_t enc){
	printf("enc.algoritm = %d\n", enc.algorithm);
	printf("enc.ciphermode = %d\n", enc.ciphermode);
	printf("enc.encryptOrDecrypt = %d\n", enc.encrypOrDecrypt);
	printf("enc.passOrKey = %d\n", enc.passOrKey);
	if(enc.passOrKey == passOrKey_key){
		printf("enc.passKeyIv.keyIv.key = %s\n", enc.passKeyIv.keyIv.key);
		printf("enc.passKeyIv.keyIv.iv = %s\n", enc.passKeyIv.keyIv.iv);
	}
	if(enc.passOrKey == passOrKey_pass){
		printf("enc.passKeyIv.password = %s\n", enc.passKeyIv.password);
	}
}

static const EVP_CIPHER *getChiper(algorithm_t algorithm,
		ciphermode_t ciphermode) {

	const fncCipher_t *modes;

	switch (algorithm) {
	case algorithm_des:
		modes = ciphers[0];
		break;
	case algorithm_aes128:
		modes = ciphers[1];
		break;
	case algorithm_aes192:
		modes = ciphers[2];
		break;
	case algorithm_aes256:
		modes = ciphers[3];
		break;
	default:
		return EVP_enc_null();

	}

	switch (ciphermode) {
	case ciphermode_cbc:
		return modes[0]();
		break;
	case ciphermode_cfb:
		return modes[1]();
		break;
	case ciphermode_ecb:
		return modes[2]();
		break;
	case ciphermode_ofb:
		return modes[3]();
		break;
	}
	return EVP_enc_null(); //NO hacer nada
}
