/* crypto.h */

#ifndef CRYPTO_H_
#define CRYPTO_H_

#include "dataHolder.h"


typedef enum {
	algorithm_none = 0, algorithm_des, algorithm_aes128, algorithm_aes192, algorithm_aes256
} algorithm_t;

typedef enum {
	ciphermode_none = 0, ciphermode_ecb, ciphermode_cfb, ciphermode_ofb, ciphermode_cbc
} ciphermode_t;

typedef enum {
	passOrKey_pass, passOrKey_key
} passOrKey_t;


typedef enum {
	encrypOrDecrypt_encrypt, encrypOrDecrypt_decrypt
} encrypOrDecrypt_t;

typedef struct {
	unsigned char *key;
	unsigned char *iv;
} keyIv_t;

typedef union {
	unsigned char *password;
	keyIv_t keyIv;
} passKeyIv_t;

typedef struct {
	algorithm_t algorithm;
	ciphermode_t ciphermode;
	passOrKey_t passOrKey;
	passKeyIv_t passKeyIv;
	encrypOrDecrypt_t encrypOrDecrypt;
} encryption_t;

typedef struct {
	encryption_t encription;
	char * sourceFile;
	char * targetFile;
} cryptoHolder_t;


/**
 *  Funci�n que encripta y desencripta segun corresponda. Es necesario construir una
 *  estructura de tipo encryptation_t que tiene todas las opciones soportadas luego
 *  un dataHolder y una direcci�n de target. El algoritmo se ocupar� de inicializar
 *  la zona de datos del holder y de setear el tama�o de los datos encryptados.
 */
int crypto_Execute(encryption_t encryptation, dataHolder_t source, dataHolder_t *target);

encryption_t newEncryptation();

int isSetCryptoAlgorithm(encryption_t enc);

int isSetCryptoCiphermode(encryption_t enc);

int isSetCryptoEncryptOrDecrypt(encryption_t enc);

int isSetCryptoPassKeyIv(encryption_t enc);

int isSetCryptoPassOrKey(encryption_t enc);

int setCryptoAlgorithm(encryption_t * enc, algorithm_t algorithm);

int setCryptoCiphermode(encryption_t * enc, ciphermode_t ciphermode);

int setCryptoEncryptOrDecrypt(encryption_t * enc, encrypOrDecrypt_t encryptOrDecrypt);

int setCryptoPassKeyIv(encryption_t * enc, passKeyIv_t passKeyIv);

int setCryptoPassOrKey(encryption_t * enc, passOrKey_t passOrKey);

int setCryptoPass(encryption_t *enc, unsigned char *pasword);

int isCryptoValid(encryption_t enc);

void cryptoShowEnc(encryption_t enc);

#endif /* CRYPTO_H_ */
