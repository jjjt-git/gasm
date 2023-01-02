#ifndef JSONHELPER_H
#define JSONHELPER_H

#include<json-c/json.h>

#define JSON_READ_BOOL(obj, key) json_object_get_boolean(json_object_object_get(obj, key))
#define JSON_READ_INT(obj, key) json_object_get_int(json_object_object_get(obj, key))

char* JSON_READ_STRING(json_object* obj, const char* key);

#endif
