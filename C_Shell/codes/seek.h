#ifndef _seek_h
#define _seek_h

#include <stdbool.h>

int seek(char *current_dir, char *reference, char *dir_name, char *target, bool d, bool f, bool e, char *next_dir, char *file, int *dir_count, int *file_count);

int special(char *home_dir, char *current_dir, char *old_dir, char *next_dir, char *file, int dir_count, int file_count, bool e);

int seek_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time);

#endif