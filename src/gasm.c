#include<stdio.h>
#include<stdlib.h>
#include<json-c/json.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>

#include"code.h"
#include"instruction.h"
#include"format.h"
#include"constants.h"
#include"errormsg.h"

#define streq(str1, str2) (strcmp(str1, str2) == 0)

int main(int argc, char* argv[]) {
	if (argc != 3) { // show help
		printf("Usage:\n\t%s <config.json> <source>\nOutput prints to stdout\nConfig search path is specified in GASM_PATH.\nGASM_PATH defaults to \".\"\nWhen source is \"--\", source will be read from stdin.\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	FILE* source;
	if (streq(argv[2], "--")) source = stdin;
	else source = fopen(argv[2], "r");
	if (source == NULL) {
		fprintf(stderr, "Error opening the source-file\n");
		exit(EXIT_FAILURE);
	}

	char* config_path_var = getenv("GASM_PATH");
	if (config_path_var == NULL) config_path_var = ".";
	char* config_path_var_ext = malloc(sizeof(char*) * (strlen(config_path_var) + 2));
	if (config_path_var_ext == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}
	strcpy(config_path_var_ext, config_path_var);
	strcat(config_path_var_ext, ":");
	char* config_path_next = strtok(config_path_var_ext, ":");
	char** config_path = NULL;
	unsigned int config_path_num = 0;
	while (config_path_next != NULL) {
		config_path_num++;
		if (config_path_num == 1) { // first
			config_path = malloc(sizeof(char*));
			if (config_path == NULL) {
				ERR_ALLOC(__FILE__,__LINE__);
			}
		} else {
			config_path = realloc(config_path, sizeof(char*) * config_path_num);
			if (config_path == NULL) {
				ERR_ALLOC(__FILE__,__LINE__);
			}
		}
		config_path[config_path_num - 1] = config_path_next;
		config_path_next = strtok(NULL, ":");
	}

	char* config_file = NULL;
	for (unsigned int ii = 0; ii < config_path_num; ii++) {
		char* dir = config_path[ii];
		char* name = malloc(sizeof(char) * (strlen(argv[1]) + strlen(dir) + 2));
		if (name == NULL) {
			ERR_ALLOC(__FILE__,__LINE__);
		}
		strcpy(name, dir);
		strcat(name, "/");
		free(dir);
		strcat(name, argv[1]);

		if (!access(name, F_OK|R_OK)) {
			// found file
			config_file = name;
			break;
		}

		free(name);
	}
	json_object* config = json_object_from_file(config_file);
	free(config_file);
	free(config_path);

	if (config == NULL) {
		fprintf(stderr, "Error reading the config-file\n");
		exit(EXIT_FAILURE);
	}

	const char* version = json_object_get_string(json_object_object_get(config, "version"));

	if (version[0] != '1' || version[1] != '.') {
		fprintf(stderr, "Unsupported version\n");
		exit(EXIT_FAILURE);
	}

	bool fixLen = false;
	unsigned int instructionLen = 0;
	instruction_bs_t instructionMask = -1;
	if (streq(version, "1.1")) {
		fixLen = true;
		instructionLen = json_object_get_int(json_object_object_get(config, "baseLength"));
		instructionMask = (1L << (instructionLen * 8)) - 1;
	}

	constants_t* spec_constants = newConstants();
	constants_t* code_constants = newConstants();

	parseConstants(json_object_object_get(config, "constants"), spec_constants, code_constants);
	instructions_t* instructions = parseInstructionSpecs(json_object_object_get(config, "instructions"),
							     spec_constants);
	freeConstants(spec_constants);

	formats_t* formats = parseFormats(json_object_object_get(config, "formats"));

	json_object_put(config);

	unsigned int lineNum = 0;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	while ((read = getline(&line, &len, source)) != -1) {
		instruction_bs_t code = translateInstruction(
				line,
				instructions,
				code_constants,
				formats,
				++lineNum
			);
		if (fixLen) {
			code = code & instructionMask;
			printf("%0*llX\n", instructionLen * 2, code);
		} else {
			printf("%llX\n", code);
		}
	}

	freeInstructions(instructions);
	freeConstants(code_constants);
	freeFormats(formats);

	return 0;
}
