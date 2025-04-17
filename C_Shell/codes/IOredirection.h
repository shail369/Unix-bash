#ifndef _IOredirection_h
#define _IOredirection_h

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include "activities.h"

#define MAX_LENGTH 4096

int proper(char *commands[], int counter, bool *input_file, bool *output_file_write, bool *output_file_append, char *input_filename, char *output_filename);

// int redirection_input(char *current_dir, char *old_dir, char *home_dir, bool output_file_append, bool output_file_write, char *input_filename, char *output_filename, char *commands[], char *command, int *time, int counter);

#endif