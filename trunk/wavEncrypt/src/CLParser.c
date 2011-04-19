#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int CLParser(int argc, char ** argv) {
	int c;

	while((c = getopt(argc, argv, "iae")) != -1)
	{
		switch (c) {
			case 'i':
				printf("iiiiiiii\n");
				break;
			case 'a':
				printf("aaaaaa\n");
				break;
			case 'e':
				printf("eeeeeeee\n");
				break;
			default:
				break;
		}
	}

	return 0;
}
