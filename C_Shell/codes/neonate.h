#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <sys/time.h>

extern int stop;

char get_keypress();

pid_t find_pid();

void neonate(int time_arg);

int neonate_main(int counter, char * commands[], char *command, int *time);

