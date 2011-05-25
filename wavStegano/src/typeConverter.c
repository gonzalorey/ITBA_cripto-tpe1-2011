/* typeConverter.c */

#include <string.h>
#include "typeConverter.h"

#define DEBUG_LOG
#include "debug.h"

void typeConvertFCtoDH(fileContainer_t  *fileContainer, dataHolder_t *dataHolder){
	LOG("typeConvertFCtoDH((fileContainer{%d, %p, %s}, dataHolder{%d, %p})\n", fileContainer->size, fileContainer->rawData, fileContainer->extension, dataHolder->size, dataHolder->data);
	dataHolder->size = sizeof(int) + fileContainer->size + strlen(fileContainer->extension);
	LOG("dataHolder->size=%d\n", dataHolder->size);
	dataHolder->data = malloc(dataHolder->size);
	((int *)dataHolder->data)[0] = fileContainer->size;
	memcpy(dataHolder->data + sizeof(int), fileContainer->rawData, fileContainer->size);
	memcpy(dataHolder->data + sizeof(int) + fileContainer->size, fileContainer->extension, strlen(fileContainer->extension));

}

void typeConvertDHtoFC(fileContainer_t  *fileContainer, dataHolder_t *dataHolder){
	LOG("typeConvertDHtoFC((fileContainer{%d, %p, %s}, dataHolder{%d, %p})\n", fileContainer->size, fileContainer->rawData, fileContainer->extension, dataHolder->size, dataHolder->data);
	int extSize;
	fileContainer->size = ((int *)dataHolder->data)[0];
	LOG("fileSize is: %d\n", fileContainer->size);
	fileContainer->rawData = malloc(fileContainer->size);
	extSize = dataHolder->size - (fileContainer->size + sizeof(int));
	LOG("extSize is: %d\n", extSize);
	memcpy(fileContainer->rawData, dataHolder->data+sizeof(int), fileContainer->size);
	memcpy(fileContainer->extension, dataHolder->data + sizeof(int) + fileContainer->size, extSize);
	fileContainer->extension[extSize] = 0;
	LOG("ext is %s\n", fileContainer->extension);

}

