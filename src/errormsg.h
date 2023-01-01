#ifndef ERRORMSG_H
#define ERRORMSG_H

#include<stdio.h>
#include<stdlib.h>

#define ERR_ALLOC(file, line) fprintf(stderr, "ERROR WHILE ALLOCATING MEMORY AT %s:%d\n", file, line); exit(EXIT_FAILURE);
#define ERR_CFG(msg) fprintf(stderr, "ERROR WHILE READING CONFIGURATION: %s\n", msg); exit(EXIT_FAILURE);
#define ERR_CONST(msg) fprintf(stderr, "ERROR WHILE SUBSTITUTING CONSTANT: %s\n", msg); exit(EXIT_FAILURE);
#define ERR_SYNTAX(msg, line) fprintf(stderr, "SYNTAX ERROR ON LINE %d, \"%s\"\n", line, msg)

#endif
