/* dataHolder.h  */

#ifndef DATAHOLDER_H_
#define DATAHOLDER_H_

#include "types.h"

typedef struct {
	BYTE *data;
	int size;
} dataHolder_t;

dataHolder_t newDataHolder(BYTE *data, int size);

void dataHolderFree(dataHolder_t dataHolder);


#endif /* DATAHOLDER_H_ */
