/* main.c */
#include <stdlib.h>
#include <stdio.h>

#include "fileContainer.h"

int
main(int argc, char *argv[]){

	int i;
	fileContainer_t orig, lsb1, lsb4, lsbe;

	if(argc < 5){
		printf("Not enough files\n");
		return 1;
	}

	if(FCRead(argv[1], &orig) != 1){
		printf("ORIG File incorrect\n");
	}
	if(FCRead(argv[2], &lsb1) != 1){
		printf("LSB1 File incorrect\n");
	}
	if(FCRead(argv[3], &lsb4) != 1){
		printf("LSB4 File incorrect\n");
	}
	if(FCRead(argv[4], &lsbe) != 1){
		printf("LSBE File incorrect\n");
	}

	int diff_lsb1 = 0, diff_lsb4 = 0, diff_lsbe = 0;
	for(i = 0; i < orig.size; i++){
		diff_lsb1 += abs(((char*)orig.rawData)[i] - ((char*)lsb1.rawData)[i]);
		diff_lsb4 += abs(((char*)orig.rawData)[i] - ((char*)lsb4.rawData)[i]);
		diff_lsbe += abs(((char*)orig.rawData)[i] - ((char*)lsbe.rawData)[i]);
	}

	printf("SIZE ORIGINAL=%d\n", orig.size);
	printf("LSB1=%f%%\n", 100 * ((float)diff_lsb1)/orig.size);
	printf("LSB4=%f%%\n", 100 * ((float)diff_lsb4)/orig.size);
	printf("LSBE=%f%%\n", 100 * ((float)diff_lsbe)/orig.size);

	return 0;
}
