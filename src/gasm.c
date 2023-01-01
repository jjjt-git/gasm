#include<stdio.h>

#include<stdio.h>
#include<stdlib.h>
#include<json-c/json.h>
#include<string.h>

#include"code.h"
#include"instruction.h"
#include"format.h"
#include"constants.h"

int main(int argc, char* argv[]) {
	if (argc != 3) { // show help
		printf("Usage:\n\t%s <config.json> <source>\nOutput prints to stdout\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	FILE* source = fopen(argv[2], "r");
	if (source == NULL) {
		fprintf(stderr, "Error opening the source-file\n");
		exit(EXIT_FAILURE);
	}

	json_object* config = json_object_from_file(argv[1]);

	if (config == NULL) {
		fprintf(stderr, "Error reading the config-file\n");
		exit(EXIT_FAILURE);
	}

	if (strcmp("1.0", json_object_get_string(json_object_object_get(config, "version"))) != 0) {
		fprintf(stderr, "Unsupported version\n");
		exit(EXIT_FAILURE);
	}

	constants_t* spec_constants = newConstants();
	constants_t* code_constants = newConstants();

	parseConstants(json_object_object_get(config, "constants"), spec_constants, code_constants);
	instructions_t* instructions = parseInstructionSpecs(json_object_object_get(config, "instructions"),
							     spec_constants);
	formats_t* formats = parseFormats(json_object_object_get(config, "formats"));

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
		printf("%llx\n", code);
	}

	return 0;
}
