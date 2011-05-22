/* main.c */
#include <stdio.h>

#define DEBUG_LOG
#include "debug.h"


int main(int argc, char *argv[]){
	LOG("entering main\n");

	printf("Hello World\n");

	LOG("exiting main\n");
	return 0;
}
