#include"jsonhelper.h"

#include<json-c/json.h>
#include<string.h>
#include<stdlib.h>

#include"errormsg.h"

char* JSON_READ_STRING(json_object* obj, const char* key) {
	char* data = json_object_get_string(json_object_object_get(obj, key));
	char* res = malloc(sizeof(char) * (strlen(data) + 1));
	if (res == NULL) {
		ERR_ALLOC(__FILE__,__LINE__);
	}
	strcpy(res, data);
	return res;
}
