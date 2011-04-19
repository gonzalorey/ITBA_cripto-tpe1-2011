/* dataHolder.h  */

#ifndef DATAHOLDER_H_
#define DATAHOLDER_H_

typedef struct {
	void *data;
	long size;
} dataHolder_t;

dataHolder_t newDataHolder(void *data, long size);

void dataHolderFree(dataHolder_t dataHolder);


#endif /* DATAHOLDER_H_ */
