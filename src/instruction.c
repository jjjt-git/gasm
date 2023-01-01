#include"instruction.h"

#include<stdlib.h>
#include<json-c/json.h>
#include<stdbool.h>

#include"errormsg.h"
#include"strhelper.h"
#include"values.h"

static inline variable_t* parseInstructionVariable(json_object *var, constants_t *constants) {
	variable_t* res = malloc(sizeof(variable_t));
	if (res == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}

	res->name = json_object_get_string(json_object_object_get(var, "name"));
	str_tolower(res->name);

	res->length = json_object_get_int(json_object_object_get(var, "length"));

	char* value = json_object_get_string(json_object_object_get(var, "value"));
	if (value[0] == '$') {
		res->isArg = 0;
		value++;
	} else {
		res->isArg = 1;
	}
	res->value = getValue(value, constants);

	return res;
}

instruction_t* parseInstructionSpec(json_object* instruction, constants_t* constants) {
	instruction_t* res = malloc(sizeof(instruction_t));
	if (res == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}

	res->name = json_object_get_string(json_object_object_get(instruction, "name"));
	res->format = json_object_get_string(json_object_object_get(instruction, "format"));
	str_tolower(res->name);
	str_tolower(res->format);

	res->argNum = json_object_get_int(json_object_object_get(instruction, "argNum"));

	json_object* vars = json_object_object_get(instruction, "variables");
	res->varNum = json_object_array_length(vars);

	res->vars = malloc(sizeof(variable_t*) * res->varNum);

	for (unsigned int ii = 0; ii < res->varNum; ii++) {
		res->vars[ii] = parseInstructionVariable(json_object_array_get_idx(vars, ii), constants);
	}

	return res;
}
