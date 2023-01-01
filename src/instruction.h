#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include<stdbool.h>
#include<json-c/json.h>

#include"types.h"

typedef struct {
	char* name;
	size_t length;
	instruction_bs_t value;
	bool isArg;
} variable_t;

typedef struct {
	char* name;
	char* format;
	size_t argNum;
	size_t varNum;
	variable_t** vars;
} instruction_t;

instruction_t* parseInstructionSpec(json_object* instruction, constants_t* constants);

#endif
