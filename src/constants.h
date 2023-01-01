#ifndef CONSTANTS_H
#define CONSTANTS_H

#include<json-c/json.h>
#include"instruction.h"

typedef struct __constants_t constants_t;

constants_t* newConstants();

void parseConstants(json_object* constants,
		    constants_t* forDefinitions, constants_t* forCode);

instruction_bs_t getConstValue(constants_t* constants, const char* name);

#endif
