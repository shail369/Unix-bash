#ifndef _reveal_h
#define reveal_h

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>

#define COLOR_BLUE "\x1b[34m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_WHITE "\x1b[37m"
#define COLOR_RESET "\x1b[0m"

#define MAX_LENGTH 4096

int compare_reveal(const void *a, const void *b);

void print_permissions(struct stat file_stat);

char *colour(struct stat file_stat);

int get_path(char *path, char *current_dir, char *old_dir, char *home_dir);

int all_details(char *current_dir, char *filepath, char *filename, bool skip);

int reveal(char *current_dir, char *path, bool a, bool l);

int reveal_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time);

#endif