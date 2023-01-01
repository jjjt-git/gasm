#include"strhelper.h"

#include<ctype.h>

void str_tolower(char* p) {
	for ( ; *p; ++p) *p = tolower(*p);
}
