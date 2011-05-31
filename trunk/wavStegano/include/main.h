/*
 * main.h */

#ifndef MAIN_H_
#define MAIN_H_

#include "crypto.h"
#include "stegano.h"
#include "crypto.h"


typedef enum {arg_in, arg_out,arg_porter, arg_embed, arg_extract, arg_pass, arg_algorithm, arg_ciphermode, arg_help, arg_steg} argId_t;

typedef enum {steg_notSet=0, steg_embed, steg_extract} steg_t;

typedef struct {
	char *sourceFile;
	char *targetFile;
	char *carrierFile;
	stegMode_t stegMode;
	encryption_t *encriptation;
	steg_t embed;
} configuration_t;

void assertSize();


configuration_t *clparser(int argc, char ** argv);

static int processArg(char **arg, char *value, char *desc);

static void showHelp();

static int initEncrypt(configuration_t *conf);

static algorithm_t getCryptoAlgorithm(char *value);

static ciphermode_t getCipherMode(char *value);

static stegMode_t getStegMode(char *value);

static void showConfig(configuration_t *conf);

static char *getStegModeStr(stegMode_t mode);

static char *getAlgorithmStr(algorithm_t algoritm);

static char *getCipherModeStr(ciphermode_t ciphermode);

static void checkConfig(configuration_t **conf);

#endif /* MAIN_H_ */
