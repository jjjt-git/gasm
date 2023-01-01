#include"instruction.h"

#include<stdlib.h>
#include<json-c/json.h>
#include<stdbool.h>
#include<string.h>

#include"errormsg.h"
#include"strhelper.h"
#include"values.h"

struct __instructions_t {
	char* name;
	instruction_t* content;
	struct __instructions_t* left;
	struct __instructions_t* right;
};

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
		res->isArg = 1;
		value++;
	} else {
		res->isArg = 0;
	}
	res->value = getValue(value, constants);

	return res;
}

static inline instruction_t* parseInstructionSpec(json_object* instruction, constants_t* constants) {
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

static inline instructions_t* newNode() {
	instructions_t* res = malloc(sizeof(instructions_t));

	if (res == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}

	res->name = NULL;
	res->content = NULL;
	res->left = NULL;
	res->right = NULL;

	return res;
}

static inline void insert(instructions_t* tree, instruction_t* data) {
	char* name = data->name;
	if (tree->name == NULL) {
		tree->name = name;
		tree->content = data;
		return;
	}

	while (1) {
		if (strcmp(name, tree->name) < 0) { // left
			if (tree->left == NULL) {
				tree->left = newNode();
				tree = tree->left;
				break;
			}
			tree = tree->left;
		} else if (strcmp(name, tree->name) > 0) { // right
			if (tree->right == NULL) {
				tree->right = newNode();
				tree = tree->right;
				break;
			}
			tree = tree->right;
		} else {
			ERR_CFG("Multiple definition of instruction!");
		}
	}

	tree->name = name;
	tree->content = data;
}

instructions_t* parseInstructionSpecs(json_object* instructions, constants_t* constants) {
	size_t numInst = json_object_array_length(instructions);

	instructions_t* res = newNode();

	for (unsigned int ii = 0; ii < numInst; ii++) {
		json_object* cur = json_object_array_get_idx(instructions, ii);

		instruction_t* ff = parseInstructionSpec(cur, constants);
		insert(res, ff);
	}

	return res;
}

instruction_t* getInstructionSpec(instructions_t *instructions, const char *name) {
	while (1) {
		if (instructions == NULL) {
			ERR_CFG("instructions does not exist!");
		}
		if (strcmp(name, instructions->name) < 0) { // left
			instructions = instructions->left;
		} else if (strcmp(name, instructions->name) > 0) { // right
			instructions = instructions->right;
		} else { // found
			return instructions->content;
		}
	}
}
