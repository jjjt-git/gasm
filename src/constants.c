#include"constants.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"errormsg.h"
#include"instruction.h"
#include"strhelper.h"
#include"values.h"
#include"jsonhelper.h"

struct __constants_t {
	char* name;
	instruction_bs_t value;
	struct __constants_t *left, *right;
};

constants_t* newConstants() {
	constants_t* res = malloc(sizeof(constants_t));

	if (res == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}

	res->name = NULL;
	res->value = 0;
	res->left = NULL;
	res->right = NULL;

	return res;
}

static inline void insert(constants_t* constants, const char* name, instruction_bs_t value) {
	if (constants->name == NULL) {
		constants->name = name;
		constants->value = value;
		return;
	}

	while (1) {
		if (strcmp(name, constants->name) < 0) { // left
			if (constants->left == NULL) {
				constants->left = newConstants();
				constants = constants->left;
				break;
			}
			constants = constants->left;
		} else if (strcmp(name, constants->name) > 0) { // right
			if (constants->right == NULL) {
				constants->right = newConstants();
				constants = constants->right;
				break;
			}
			constants = constants->right;
		} else {
			ERR_CFG("Multiple definition of constant!");
		}
	}

	constants->name = name;
	constants->value = value;
}

void parseConstants(json_object* constants, constants_t* spec, constants_t* code) {
	size_t numConstants = json_object_array_length(constants);

	for (unsigned int ii = 0; ii < numConstants; ii++) {
		json_object* cur = json_object_array_get_idx(constants, ii);

		char* name = JSON_READ_STRING(cur, "name");
		char* valueS = JSON_READ_STRING(cur, "value");
		str_tolower(name);
		str_tolower(valueS);

		instruction_bs_t value = getValue(valueS, NULL);
		free(valueS);

		if (JSON_READ_BOOL(cur, "inSpec")) {
			insert(spec, name, value);
		}
		if (JSON_READ_BOOL(cur, "inCode")) {
			insert(code, name, value);
		}
	}
}

instruction_bs_t getConstValue(constants_t *constants, const char *name) {
	while (1) {
		if (constants == NULL) {
			ERR_CONST("Does not exist!");
		}
		if (strcmp(name, constants->name) < 0) { // left
			constants = constants->left;
		} else if (strcmp(name, constants->name) > 0) { // right
			constants = constants->right;
		} else { // found
			return constants->value;
		}
	}
}

bool hasConstant(constants_t *constants, const char  *name) {
	if (name == NULL) return false;
	while (constants != NULL) {
		if (constants->name == NULL) return false; // empty constants
		if (strcmp(name, constants->name) < 0) { // left
			constants = constants->left;
		} else if (strcmp(name, constants->name) > 0) { // right
			constants = constants->right;
		} else { // found
			return true;
		}
	}
	return false;
}

void freeConstants(constants_t* c) {
	if (c == NULL) return;
	free(c->name);
	freeConstants(c->left);
	freeConstants(c->right);
	free(c);
}
