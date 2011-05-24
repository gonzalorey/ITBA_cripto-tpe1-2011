/* fileContainer.h */

#ifndef FILECONTAINER_H_
#define FILECONTAINER_H_

#define FILE_EXTENTION_LENGTH 8

typedef struct {
	int size;
	void *rawData;
	char extension[FILE_EXTENTION_LENGTH];
} fileContainer_t;

int FCWrite(char *path, fileContainer_t *container);

int FCRead(char *path, fileContainer_t *container);

#endif /* FILECONTAINER_H_ */
