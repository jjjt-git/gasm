#include"code.h"

#include<string.h>

#include"instruction.h"
#include"format.h"
#include"constants.h"
#include"strhelper.h"
#include"errormsg.h"
#include"values.h"

instruction_bs_t translateInstruction(const char* line,
		instructions_t* instructions,
		constants_t* constants,
		formats_t* formats,
		unsigned int lineNum) {
	str_tolower(line);

	// "INSTNAME ARG1,ARG2,ARG3,...,ARGN"
	char* name = strtok(line, " ");
	char* args = strtok(NULL, " ");
	char* arg_next = strtok(args, ",");
	char** argv = NULL;
	unsigned int argc = 0;
	while (arg_next != NULL) {
		argc++;
		if (argc == 1) { // first
			argv = malloc(sizeof(char*));
			if (argv == NULL) {
				ERR_ALLOC(__FILE__,__LINE__);
			}
		} else {
			argv = realloc(argv, sizeof(char*) * argc);
			if (argv == NULL) {
				ERR_ALLOC(__FILE__,__LINE__);
			}
		}
		argv[argc - 1] = arg_next;
		arg_next = strtok(NULL, ",");
	}

	instruction_t* inst = getInstructionSpec(instructions, name);

	if (argc != inst->argNum) {
		ERR_SYNTAX("arg-count does not match", lineNum);
	}

	format_t* format = getFormat(formats, inst->format);

	instruction_bs_t* values = malloc(sizeof(instruction_bs_t) * format->variableNum);

	for (unsigned int ii = 0; ii < format->variableNum; ii++) {
		variable_t* variable = NULL;
		for (unsigned int jj = 0; jj < inst->varNum; jj++) { // get variable
			if (strcmp(inst->vars[jj]->name, format->variables[ii]) == 0) {
				variable = inst->vars[jj];
				break;
			}
		}

		if (variable->isArg) {
			values[ii] = getValue(argv[variable->value], constants);
		} else {
			values[ii] = variable->value;
		}
	}

	free(argv);

	instruction_bs_t res = fillFormat(format, values);

	free(values);

	return res;
}
