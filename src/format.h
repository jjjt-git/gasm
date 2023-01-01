#ifndef FORMAT_H
#define FORMAT_H

#include<stdbool.h>
#include<stdarg.h>

#include<json-c/json.h>

typedef struct {
	unsigned int from, to, sliceFrom, sliceTo;
	int variable;
	bool isSlice;
} field_t;

typedef struct {
	char* name;
	unsigned int numFields;
	field_t** fields;
	char** variables; // variable map
	int* nums; // variable map
	size_t variableNum;
} format_t;

format_t* parseFormat(json_object* format);
field_t* parseField(json_object* field, int* mapLength, char** mapStrings, int* mapValues);

int fillFormat(format_t *format, ...);

#endif
