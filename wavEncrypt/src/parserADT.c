#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "parserADT.h"
#include "debug.h"

#define ARGS_BLOCK 10

#define ERROR_WRONG_ARGUMENT(arg_name)		printf("Error: Wrong argument '%s' received\n", arg_name)
#define ERROR_MISSING_ARGUMENT(arg_name)		printf("Error: Argument '%s' missing\n", arg_name)

typedef struct{
	int id;
	char * name;
	int isOptional;
	int ocurrences;
	arg_t type;
	int optionsCount;
	char ** options;
}arg;

struct parserCDT{
	int clArgc;
	char ** clArgv;
	arg ** args;
	int argsIndex;
	int argsCount;
	char * curVal;
};

arg * newArg(int id, char * name, int isOptional, int ocurrences, arg_t type, int optionsCount, char ** options);
void freeArg(arg * a);
char * setValue(parserADT parser, int argId);
char ** newOptions(int optionsCount, char ** options);

parserADT
newParser(int argc, char ** argv){
	parserADT parser;

	if((parser = malloc(sizeof(struct parserCDT))) == NULL){
		return NULL;
	}

	parser->clArgc = argc;
	parser->clArgv = argv;

	// reserve the space for the initial args block size
	if((parser->args = malloc(sizeof(arg *) * ARGS_BLOCK)) == NULL){
		free(parser);
		return NULL;
	}

	// the index is set next to the program name
	parser->argsIndex = 1;

	// the number of args starts as zero
	parser->argsCount = 0;

	// set the current val as NULL
	parser->curVal = NULL;

	return parser;
}

void
freeParser(parserADT parser){
	int i;
	for(i = 0; i < parser->argsCount; i++){
		freeArg(parser->args[i]);
	}

	free(parser->args);
	free(parser);
}

arg *
newArg(int id, char * name, int isOptional, int ocurrences, arg_t type, int optionsCount, char ** options){
	arg * ans;
	if((ans = malloc(sizeof(arg))) == NULL)
		return NULL;

	if((ans->name = malloc(strlen(name))) == NULL){
		free(ans);
		return NULL;
	}

	ans->optionsCount = optionsCount;
	if(optionsCount > 0){
		if((ans->options = newOptions(optionsCount, options)) == NULL){
			free(ans->name);
			free(ans);
			return NULL;
		}
	}

	ans->id = id;
	strcpy(ans->name, name);
	ans->isOptional = isOptional;
	ans->ocurrences = ocurrences;
	ans->type = type;

	return ans;
}

char **
newOptions(int optionsCount, char ** options){
	char ** ans;

	if((ans = malloc(sizeof(char *) * optionsCount)) == NULL){
		return NULL;
	}

	int i;
	for(i = 0; i < optionsCount; i++){
		if((ans[i] = malloc(sizeof(char *) * strlen(options[i]))) == NULL){
			int j;
			for(j = 0; j < i; j++)
				free(ans[j]);
			free(ans);
			return NULL;
		}
		strcpy(ans[i], options[i]);
	}

	return ans;
}

void
freeArg(arg * a){
	free(a->name);
	free(a);
}

int
setArg(parserADT parser, int id, char * name, int isOptional, int ocurrences, arg_t type, int optionsCount, char ** options){
	arg * a = newArg(id, name, isOptional, ocurrences, type, optionsCount, options);
	arg ** aux;

	if(a == NULL)
		return -1;

	// if there's no left space, reallocate more
	if(parser->argsCount != 0 && (parser->argsCount % ARGS_BLOCK) == 0){
		if((aux= realloc(parser->args, parser->argsCount + sizeof(arg *) * ARGS_BLOCK)) == NULL)
			return -1;
		parser->args = aux;
	}

	parser->args[parser->argsCount++] = a;

	return 0;
}

int
getArg(parserADT parser){
	int i;

	if(parser->argsIndex >= parser->clArgc)
		return -1;

	for(i = 0; i < parser->argsCount; i++){
		if(strcmp(parser->clArgv[parser->argsIndex], parser->args[i]->name) == 0){
			// if the arg doesn't have any more ocurrences left, an error is returned
			if(parser->args[i]->ocurrences <= 0)
				return -1;

			// decrement the number of remaining ocurrences
			parser->args[i]->ocurrences--;

			// if it isn't of TYPE0, free the old current value and set the new one
			if(parser->curVal != NULL){
				free(parser->curVal);
				parser->curVal = NULL;
			}

			if(parser->args[i]->type != ARG_TYPE0){
				if((parser->curVal = setValue(parser, i)) == NULL)
					return -1;

				if(parser->args[i]->optionsCount > 0){
					int j;
					int found = 0;
					for(j = 0; j < parser->args[i]->optionsCount; j++){
						if(strcmp(parser->curVal, parser->args[i]->options[j]) == 0)
							found = 1;
					}

					if(!found)
						return -1;
				}
			}

			// an arg was found, increment the index
			parser->argsIndex++;

			return parser->args[i]->id;
		}
	}

	return -1;
}

char *
setValue(parserADT parser, int argId){
	char * ans = NULL;

	switch (parser->args[argId]->type) {
		case ARG_TYPE0: // -arg
			return NULL;

		case ARG_TYPE1:	// -arg value
			if(parser->clArgc < parser->argsIndex + 1)
				return NULL;

			if((ans = malloc(strlen(parser->clArgv[parser->argsIndex + 1]))) == NULL)
				return NULL;

			strcpy(ans, parser->clArgv[parser->argsIndex + 1]);
			parser->argsIndex++;
			break;

		case ARG_TYPE2:	// -argvalue
			// not implemented
			break;

		case ARG_TYPE3:		// -arg=value
			// not implemented
			break;

		default:
			// error, do nothing
			break;
	}

	return ans;
}

char *
getValue(parserADT parser){
	return parser->curVal;
}

int
noMoreArgs(parserADT parser){
	return (parser->clArgc - parser->argsIndex) == 0;
}

int
checkErrors(parserADT parser){
	int i;
	for(i = 0; i < parser->argsCount; i++){
		if(parser->args[i]->ocurrences > 0 && !parser->args[i]->isOptional){
			ERROR_MISSING_ARGUMENT(parser->args[i]->name);
			return -1;
		}
	}

	return 0;
}
