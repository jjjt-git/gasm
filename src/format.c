#include"format.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<json-c/json_object.h>

#include"errormsg.h"
#include"strhelper.h"

format_t* parseFormat(json_object* format) {
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

field_t* parseField(json_object* field, int* mapLength, char** mapStrings) {
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

instruction_bs_t fillFormat(format_t *format, int vars[]) {
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
