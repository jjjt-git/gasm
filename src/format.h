#ifndef FORMAT_H
#define FORMAT_H

#include<stdbool.h>

#include<json-c/json.h>

#include"instruction.h"

typedef struct {
	unsigned int from, to, sliceFrom;
	int variable;
} field_t;

typedef struct {
	char* name;
	unsigned int numFields;
	field_t** fields;
	char** variables; // variable map (index -> var)
	unsigned int variableNum;
} format_t;

typedef struct __formats_t formats_t;

formats_t* parseFormats(json_object* formats);

instruction_bs_t fillFormat(format_t *format, instruction_bs_t varv[]); // returns the bitstring

format_t* getFormat(formats_t* formats, const char* name);

void freeFormats(formats_t* formats);

#endif
