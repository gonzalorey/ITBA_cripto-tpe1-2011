/* typeConverter.c */

#include <string.h>
#include "types.h"
#include "typeConverter.h"

#define DEBUG_LOG
#include "debug.h"

void reverseInt(intConvert_t *number);

void typeConvertFCtoDH(fileContainer_t  *fileContainer, dataHolder_t *dataHolder){
	intConvert_t org;
	LOG("typeConvertFCtoDH((fileContainer{%d, %p, %s}, dataHolder{%d, %p})\n", fileContainer->size, fileContainer->rawData, fileContainer->extension, dataHolder->size, dataHolder->data);
	org.number = fileContainer->size;
	reverseInt(&org);

	dataHolder->size = sizeof(int) + fileContainer->size + strlen(fileContainer->extension);
	LOG("dataHolder->size=%d\n", dataHolder->size);
	dataHolder->data = malloc(dataHolder->size);
	((int *)dataHolder->data)[0] = org.number;
	memcpy(dataHolder->data + sizeof(int), fileContainer->rawData, fileContainer->size);
	memcpy(dataHolder->data + sizeof(int) + fileContainer->size, fileContainer->extension, strlen(fileContainer->extension));

}

void typeConvertDHtoFC(fileContainer_t  *fileContainer, dataHolder_t *dataHolder){
	intConvert_t org;


	LOG("typeConvertDHtoFC((fileContainer{%d, %p, %s}, dataHolder{%d, %p})\n", fileContainer->size, fileContainer->rawData, fileContainer->extension, dataHolder->size, dataHolder->data);
	int extSize;
	org.number = ((int *)dataHolder->data)[0];
	reverseInt(&org);
	fileContainer->size = org.number;
	LOG("fileSize is: %d\n", fileContainer->size);
	fileContainer->rawData = malloc(fileContainer->size);
	extSize = dataHolder->size - (fileContainer->size + sizeof(int));
	LOG("extSize is: %d\n", extSize);
	memcpy(fileContainer->rawData, dataHolder->data+sizeof(int), fileContainer->size);
	memcpy(fileContainer->extension, dataHolder->data + sizeof(int) + fileContainer->size, extSize);
	fileContainer->extension[extSize] = 0;
	LOG("ext is %s\n", fileContainer->extension);

}

void reverseInt(intConvert_t *number){
	char aux;

	printf("%d\n", number->number);

	aux = number->bytes[3];
	number->bytes[3] = number->bytes[0];
	number->bytes[0] = aux;

	aux = number->bytes[1];
	number->bytes[1] = number->bytes[2];
	number->bytes[2] = aux;

	printf("%d\n", number->number);

}

