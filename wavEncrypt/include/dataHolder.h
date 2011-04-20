/* dataHolder.h  */

#ifndef DATAHOLDER_H_
#define DATAHOLDER_H_

typedef struct {
	void *data;
	int size;
} dataHolder_t;

dataHolder_t newDataHolder(void *data, int size);

void dataHolderFree(dataHolder_t dataHolder);


#endif /* DATAHOLDER_H_ */
