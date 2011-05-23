/* fileHolder.h */

#ifndef FILEHOLDER_H_
#define FILEHOLDER_H_

#include "types.h"

#define FILE_EXT 8

typedef struct {
	int size;
	byte *rawData;
	char ext[FILE_EXT]; //.ext\0
} fileHolder_t;


int fileHolderLoad(char *path, fileHolder_t *holder);

int fileHolderSave(char *path, fileHolder_t *holder);

#endif /* FILEHOLDER_H_ */
