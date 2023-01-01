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

format_t* parseFormat(json_object* format);

instruction_bs_t fillFormat(format_t *format, int varv[]); // returns the bitstring

#endif
