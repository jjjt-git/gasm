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

typedef struct __instructions_t instructions_t;

instructions_t* parseInstructionSpecs(json_object* instruction, constants_t* constants);
instruction_t* getInstructionSpec(instructions_t* instructions, const char* name);

void freeInstructions(instructions_t* instructions);

#endif
