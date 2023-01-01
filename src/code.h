#ifndef CODE_H
#define CODE_H

#include"instruction.h"
#include"format.h"
#include"constants.h"

instruction_bs_t translateInstruction(const char* line,
	instructions_t* instructions,
	constants_t* constants,
	formats_t* formats,
	unsigned int lineNum);

#endif
