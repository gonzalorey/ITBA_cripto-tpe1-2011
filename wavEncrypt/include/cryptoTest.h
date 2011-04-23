/*
 * cryptoTest.h
 *
 *  Created on: Apr 23, 2011
 *      Author: anizzomc
 */

#ifndef CRYPTOTEST_H_
#define CRYPTOTEST_H_

#include "crypto.h"

void cryptoRunTests(void);

void testInit(void);

void testDesEcb(void);
void testDesCfb(void);
void testDesOfb(void);
void testDesCbc(void);

void testAes128Ecb(void);
void testAes128Cfb(void);
void testAes128Ofb(void);
void testAes128Cbc(void);

void testAes192Ecb(void);
void testAes192Cfb(void);
void testAes192Ofb(void);
void testAes192Cbc(void);

void testAes256Ecb(void);
void testAes256Cfb(void);
void testAes256Ofb(void);
void testAes256Cbc(void);

void testAlgorithm(char *name, algorithm_t algorithm, ciphermode_t ciphermode);

#endif /* CRYPTOTEST_H_ */
