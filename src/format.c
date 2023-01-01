#include"format.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<json-c/json_object.h>

#include"errormsg.h"
#include"strhelper.h"

struct __formats_t {
	char* name;
	format_t* content;
	struct __formats_t* left;
	struct __formats_t* right;
};

static inline field_t* parseField(json_object* field, int* mapLength, char** mapStrings) {
	int from = json_object_get_int(json_object_object_get(field, "from"));
	int to = json_object_get_int(json_object_object_get(field, "to"));
	const char* variableName = json_object_get_string(json_object_object_get(field, "variable"));
	bool isSlice = json_object_get_boolean(json_object_object_get(field, "isSlice"));

	str_tolower(variableName);

	int sFrom = 0;

	if (isSlice) {
		sFrom = json_object_get_int(json_object_object_get(field, "sliceFrom"));
	}

	int varMapNum = -1;
	for (int ii = 0; ii < *mapLength; ii++) {
		if (strcmp(variableName, mapStrings[ii]) == 0) {
			varMapNum = ii;
			break;
		}
	}

	if (varMapNum == -1) {
		mapStrings[*mapLength] = variableName;
		varMapNum = *mapLength;
		(*mapLength)++;
	}

	field_t* res = malloc(sizeof(field_t));
	if (res == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}

	res->from = from;
	res->to = to;
	res->sliceFrom = sFrom;
	res->variable = varMapNum;

	return res;
}

static inline format_t* parseFormat(json_object* format) {
	json_object* json_fields = json_object_object_get(format, "fields");
	size_t numFields = json_object_array_length(json_fields);

	const char* name = json_object_get_string(json_object_object_get(format, "name"));

	str_tolower(name);

	field_t** fields = malloc(sizeof(field_t) * numFields);
	char** mapStrings = malloc(sizeof(char*) * numFields);

	if (mapStrings == NULL || fields == NULL) {
		ERR_ALLOC(__FILE__, __LINE__);
	}

	int mapLength = 0;

	for (unsigned int ii = 0; ii <= numFields; ii++) {
		fields[ii] = parseField(json_object_array_get_idx(json_fields, ii),
				&mapLength, mapStrings);
	}

	mapStrings = realloc(mapStrings, sizeof(char*) * mapLength);
	format_t* res = malloc(sizeof(format_t));

	if (mapStrings == NULL || res == NULL) {
		ERR_ALLOC(__FILE__, __LINE__);
	}


	res->name = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(res->name, name);

	res->numFields = numFields;
	res->variableNum = mapLength;

	res->fields = fields;
	res->variables = mapStrings;

	return res;
}

instruction_bs_t fillFormat(format_t *format, instruction_bs_t vars[]) {
	instruction_bs_t res = 0;

	for (unsigned int ii = 0; ii < format->numFields; ii++) {
		field_t* field = format->fields[ii];
		instruction_bs_t iMask = 0;
		for (unsigned int maskMaker = field->from; maskMaker <= field->to; maskMaker++) {
			iMask = iMask | (1 << maskMaker);
		}

		instruction_bs_t data = vars[field->variable] << (field->from - field->sliceFrom);

		res = res | (iMask & data);
	}

	return res;
}

static inline formats_t* newNode() {
	formats_t* res = malloc(sizeof(formats_t));

	if (res == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}

	res->name = NULL;
	res->content = NULL;
	res->left = NULL;
	res->right = NULL;

	return res;
}

static inline void insert(formats_t* tree, format_t* data) {
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
			ERR_CFG("Multiple definition of format!");
		}
	}

	tree->name = name;
	tree->content = data;
}

formats_t* parseFormats(json_object* formats) {
	size_t numFormats = json_object_array_length(formats);

	formats_t* res = newNode();

	for (unsigned int ii = 0; ii < numFormats; ii++) {
		json_object* cur = json_object_array_get_idx(formats, ii);

		format_t* ff = parseFormat(cur);
		insert(res, ff);
	}

	return res;
}

format_t* getFormat(formats_t *formats, const char *name) {
	while (1) {
		if (formats == NULL) {
			ERR_CFG("Format does not exist!");
		}
		if (strcmp(name, formats->name) < 0) { // left
			formats = formats->left;
		} else if (strcmp(name, formats->name) > 0) { // right
			formats = formats->right;
		} else { // found
			return formats->content;
		}
	}
}
