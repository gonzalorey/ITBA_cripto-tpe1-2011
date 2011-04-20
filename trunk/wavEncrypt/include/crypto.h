/* crypto.h */

#ifndef CRYPTO_H_
#define CRYPTO_H_

#include "dataHolder.h"


typedef enum {
	algorithm_des, algorithm_aes, algorithm_aes128, algorithm_aes256
} algorithm_t;

typedef enum {
	ciphermode_ecb, ciphermode_cfb, ciphermode_ofb, ciphermode_cbc
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
	char *password;
	keyIv_t keyIv;
} passKeyIv_t;

typedef struct {
	algorithm_t algorithm;
	ciphermode_t ciphermode;
	passOrKey_t passOrKey;
	passKeyIv_t passKeyIv;
	encrypOrDecrypt_t encrypOrDecrypt;
} encryption_t;


/**
 *  Funci—n que encripta y desencripta segun corresponda. Es necesario construir una
 *  estructura de tipo encryptation_t que tiene todas las opciones soportadas luego
 *  un dataHolder y una direcci—n de target. El algoritmo se ocupar‡ de inicializar
 *  la zona de datos del holder y de setear el tama–o de los datos encryptados.
 */
int cripto_Execute(encryption_t encryptation, dataHolder_t source, dataHolder_t *target);

#endif /* CRYPTO_H_ */
