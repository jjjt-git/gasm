#ifndef ERRORMSG_H
#define ERRORMSG_H

#include<stdio.h>
#include<stdlib.h>

#define ERR_ALLOC(file, line) fprintf(stderr, "ERROR WHILE ALLOCATING MEMORY AT %s:%d\n", file, line); exit(EXIT_FAILURE);

#endif
