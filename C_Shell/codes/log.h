#ifndef _log_h
#define _log_h

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include "activities.h"

#define MAX_LENGTH 4096

int existing(char *current[], char *current_dir, char *home_dir);

int store(char *command, char *current_dir, char *home_dir);

int history(char *current_dir, char *home_dir);

int clear(char *current_dir, char *home_dir);

int execute(int index, char *current_dir, char *home_dir, char *old_dir, char *commands_print, int *time);

int log_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time);

#endif