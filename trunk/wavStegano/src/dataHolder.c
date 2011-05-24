/* dataHolder.c  */

#include "dataHolder.h"

#include <stdlib.h>

dataHolder_t newDataHolder(BYTE *data, int size) {
	dataHolder_t holder;

	holder.data = data;
	holder.size = size;

	return holder;
}

void dataHolderFree(dataHolder_t dataHolder) {
	free(dataHolder.data);
}
