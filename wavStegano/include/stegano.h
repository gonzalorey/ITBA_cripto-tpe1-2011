/* stegano.h */

#ifndef STEGANO_H_
#define STEGANO_H_

typedef enum {stegMode_LSB, stegMode_LSB4, stegMode_LSBE} stegMode_t;
typedef enum {stegResult_Success, stegResult_fail,  stegResult_memoryFail, stegResult_sizeFail} stegResult_t;

stegResult_t stegEmbed(void *carrier, long carrierSize, void *message, long messageSize, stegMode_t mode);

stegResult_t stegExtract(void *carrier, long carrierSize, void *message, long messageSize, stegMode_t mode);

long stegSizeRequired(long messageSize, stegMode_t mode);

#endif /* STEGANO_H_ */
