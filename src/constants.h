#ifndef CONSTANTS_H
#define CONSTANTS_H

#include<json-c/json.h>
#include<stdbool.h>

#include"instruction.h"
#include"types.h"

constants_t* newConstants();

void parseConstants(json_object* constants,
		    constants_t* forDefinitions, constants_t* forCode);

instruction_bs_t getConstValue(constants_t* constants, const char* name);
bool hasConstant(constants_t* constants, const char* name);

void freeConstants(constants_t* constants);

#endif
