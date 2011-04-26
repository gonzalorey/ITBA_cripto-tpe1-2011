/*
 * cryptoTest.c
 *
 *  Created on: Apr 22, 2011
 *      Author: anizzomc
 */


#include "dataHolder.h"
#include "cryptoTest.h"
#include "crypto.h"
#include <string.h>

#define DEBUG_LOG
#include "debug.h"

static char
		*plainText =
				"Some test to encrypt, choose what ever you want to encrypt.\nThe final Result should be the same....";

static encryption_t enc;

static dataHolder_t source;

void cryptoRunTests(){
	testInit();
	testDesEcb();
	testDesCfb();
	testDesOfb();
	testDesCbc();

	testAes128Ecb();
	testAes128Cfb();
	testAes128Ofb();
	testAes128Cbc();

	testAes192Ecb();
	testAes192Cfb();
	testAes192Ofb();
	testAes192Cbc();

	testAes256Ecb();
	testAes256Cfb();
	testAes256Ofb();
	testAes256Cbc();

}


void testInit() {
	passKeyIv_t passKeyIv;

	passKeyIv.keyIv.iv = (unsigned char*) "12345678901234567890123456789012";
	passKeyIv.keyIv.key = (unsigned char*)"01234567890123456789012345678901";

	enc = newEncryptation();
	setCryptoPassOrKey(&enc, passOrKey_key);
	setCryptoPassKeyIv(&enc, passKeyIv);

	source.data = plainText;
	source.size = strlen(plainText);

}

void testAlgorithm(char *name, algorithm_t algorithm, ciphermode_t ciphermode) {

	dataHolder_t target;
	dataHolder_t decrypt;

	setCryptoEncryptOrDecrypt(&enc, encrypOrDecrypt_encrypt);
	setCryptoAlgorithm(&enc, algorithm);
	setCryptoCiphermode(&enc, ciphermode);

	LOG("Starting %s test\n", name);

	if (!crypto_Execute(enc, source, &target)) {
		ERROR("%s encryption failed\n", name);
		return;
	}

	setCryptoEncryptOrDecrypt(&enc, encrypOrDecrypt_decrypt);

	if (!crypto_Execute(enc, target, &decrypt)) {
		ERROR("testDesEcb dencryption failed\n");
		return;
	}

	if (decrypt.size != source.size) {
		ERROR("%s decrypt=(%d) and source=(%d) data sizes don't match\n", name, decrypt.size, source.size);
		return;
	}

	if (memcmp(source.data, decrypt.data, decrypt.size)) {
		ERROR("%s decrypt and source data don't match\n", name);
		return;
	}

	dataHolderFree(target);
	dataHolderFree(decrypt);

	LOG("%s success\n", name);
	LOG("Finished %s test\n", name);

}

void testDesEcb() {
	testAlgorithm("testDesEcb", algorithm_des, ciphermode_ecb);
}

void testDesCfb() {
	testAlgorithm("testDesCfb", algorithm_des, ciphermode_ecb);
}

void testDesOfb() {
	testAlgorithm("testDesOfb", algorithm_des, ciphermode_ofb);
}

void testDesCbc() {
	testAlgorithm("testDesCbc", algorithm_des, ciphermode_cbc);
}

void testAes128Ecb() {
	testAlgorithm("testAes128Ecb", algorithm_aes128, ciphermode_ecb);
}

void testAes128Cfb() {
	testAlgorithm("testAes128Cfb", algorithm_aes128, ciphermode_cfb);
}

void testAes128Ofb() {
	testAlgorithm("testAes128Ofb", algorithm_aes128, ciphermode_ofb);
}

void testAes128Cbc() {
	testAlgorithm("testAes128Cbc", algorithm_aes128, ciphermode_cbc);
}

void testAes192Ecb() {
	testAlgorithm("testAes192Ecb", algorithm_aes192, ciphermode_ecb);
}

void testAes192Cfb() {
	testAlgorithm("testAes192Cfb", algorithm_aes192, ciphermode_cfb);
}

void testAes192Ofb() {
	testAlgorithm("testAes192Ofb", algorithm_aes192, ciphermode_ofb);
}

void testAes192Cbc() {
	testAlgorithm("testAes192Cbc", algorithm_aes192, ciphermode_cbc);
}

void testAes256Ecb() {
	testAlgorithm("testAes256Ecb", algorithm_aes256, ciphermode_ecb);
}

void testAes256Cfb() {
	testAlgorithm("testAes256Cfb", algorithm_aes256, ciphermode_cfb);

}

void testAes256Ofb() {
	testAlgorithm("testAes256Ofb", algorithm_aes256, ciphermode_ofb);
}

void testAes256Cbc() {
	testAlgorithm("testAes256Cbc", algorithm_aes256, ciphermode_cbc);
}
