#ifndef _helper_h
#define _helper_h

#include <sys/utsname.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
#include "activities.h"

#define MAX_LENGTH 4096
#define MAX_PROCESSES 100

extern int foreground_pid;
extern int stdin_fd;
extern int stdout_fd;
extern int stop;

extern char *alias[MAX_PROCESSES];
extern char *alias_function[MAX_PROCESSES];

void get_details(char *username, char *systemname);

void get_prompt(char *current_dir, char *home_dir, char *username, char *systemname, char *command, int time);

char *trim_whitespace(char *str);

void aliases();

int multiple(char *original_command, int counter_and[], int counter_individual[], int counter_pipe[], char *individual_commands[]);

void cleanup_variables(char *individual_commands[], int num_individual_commands);

int open_output_file(char *output_filename, int flags, int mode);

int set_stdout(int redirect_output, bool condition);

int set_stdout(int redirect_output, bool condition);

int reset_stdout(int stdout_fd, bool condition);

int reset_stdin(int stdin_fd, bool condition);

bool is_background(int *counter, int *index_and, bool has_pipe);

void next_iteration(int *i, int counter, int *index_individual, int num_individual_commands, char *commands[], char *individual_commands[]);

int alias_replacement(int index, char *commands[], int *counter);

int solve(char *original_command, char *current_dir, char *home_dir, char *old_dir, char *command, int *time);

#endif