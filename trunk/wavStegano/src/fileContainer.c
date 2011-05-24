/* fileContainer.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "fileContainer.h"


int FCRead(char *path, fileContainer_t *container) {
	FILE *source;
	int retVal = 0;
	char name[64];

	if((source = fopen(path, "r")) != NULL){
		struct stat st;
		stat(path, &st);
		container->size = st.st_size;

		sscanf(path, "%[^.]%s", name, container->extension);

		if((container->rawData = malloc(container->size)) == NULL){
			fclose(source);
			return 0;
		}

		if(fread(container->rawData, container->size, 1, source) == 0){
			free(container->rawData);
			container->rawData = NULL;
			retVal = 0;
		} else {
			retVal = 1;
		}

		fclose(source);
	}

	return retVal;



}

int FCWrite(char *path, fileContainer_t *container) {
	FILE *target;
	int result = 0;

	if((target = fopen(path, "w")) != NULL) {
		if(fwrite(container->rawData, container->size, 1, target) != 0){
			result = 1;
		}
		fclose(target);
	}

	return result;
}
