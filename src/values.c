#include"values.h"

#include<stdlib.h>

#include"constants.h"
#include"instruction.h"

instruction_bs_t getValue(const char* data, constants_t* constants) {
	instruction_bs_t value;

	if (constants == NULL || !hasConstant(constants, data)) {
		if (data[0] == 'x') {
			value = strtol(data + 1, NULL, 16);
		} else if (data[0] == 'b') {
			value = strtol(data + 1, NULL, 2);
		} else if (data[0] == 'o') {
			value = strtol(data + 1, NULL, 8);
		} else {
			value = strtol(data, NULL, 10);
		}
	} else {
		value = getConstValue(constants, data);
	}
	return value;
}
