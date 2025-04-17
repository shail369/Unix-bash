#ifndef _myshrc_h_
#define _myshrc_h_
#include "helper.h"

extern char *alias[MAX_PROCESSES];
extern char *alias_function[MAX_PROCESSES];

char *trim_whitespace(char *str);

void aliases();

int alias_replacement(int index, char *commands[], int *counter);

#endif