typedef struct parserCDT * parserADT;

typedef enum {
	ARG_TYPE0,		// -arg
	ARG_TYPE1,		// -arg value
	ARG_TYPE2,		// -argvalue
	ARG_TYPE3		// -arg=value
}arg_t;

parserADT newParser(int argc, char ** argv);

void freeParser(parserADT parser);

int setArg(parserADT parser, int id, char * name, int isOptional, int ocurrences, arg_t type, int optionsCount, char ** options);

int getArg(parserADT parser);

char * getValue(parserADT parser);

int noMoreArgs(parserADT parser);

int checkErrors(parserADT parser);
