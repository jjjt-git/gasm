#include"format.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<json-c/json_object.h>

#include"errormsg.h"

format_t* parseFormat(json_object* format) {
	json_object* json_fields = json_object_object_get(format, "fields");
	size_t numFields = json_object_array_length(json_fields);

	const char* name = json_object_get_string(json_object_object_get(format, "name"));

	field_t** fields = malloc(sizeof(field_t) * numFields);
	char** mapStrings = malloc(sizeof(char*) * numFields);
	int* mapValues = malloc(sizeof(int) * numFields);

	if (mapStrings == NULL || mapValues == NULL || fields == NULL) {
		ERR_ALLOC(__FILE__, __LINE__);
	}

	int mapLength = 0;

	for (unsigned int ii = 0; ii <= numFields; ii++) {
		fields[ii] = parseField(json_object_array_get_idx(json_fields, ii),
				&mapLength, mapStrings, mapValues);
	}

	mapStrings = realloc(mapStrings, sizeof(char*) * mapLength);
	mapValues = realloc(mapValues, sizeof(int) * mapLength);
	format_t* res = malloc(sizeof(format_t));

	if (mapStrings == NULL || mapValues == NULL || res == NULL) {
		ERR_ALLOC(__FILE__, __LINE__);
	}


	res->name = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(res->name, name);

	res->numFields = numFields;
	res->variableNum = mapLength;

	res->fields = fields;
	res->variables = mapStrings;
	res->nums = mapValues;

	return res;
}
