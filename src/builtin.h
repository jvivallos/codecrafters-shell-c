#ifndef _BUILTIN_H_
#define _BUILTIN_H_
#include <stdio.h>

int is_builtin(char *command);
int execute_type(char *command, size_t size);
void execute_builtin(char *command, size_t size);

#endif