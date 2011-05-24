/* stegano.h */

#ifndef STEGANO_H_
#define STEGANO_H_

#include "dataHolder.h"

typedef enum {stegMode_LSB, stegMode_LSB4, stegMode_LSBE} stegMode_t;
typedef enum {stegResult_Success, stegResult_fail,  stegResult_memoryFail, stegResult_sizeFail} stegResult_t;

stegResult_t stegEmbed(dataHolder_t *carrier, dataHolder_t *payload, stegMode_t mode, char *extention);

stegResult_t stegExtract(dataHolder_t *carrier, dataHolder_t *payload, stegMode_t mode, char *extention);

long stegSizeRequired(long payloadSize, stegMode_t mode);

#endif /* STEGANO_H_ */
