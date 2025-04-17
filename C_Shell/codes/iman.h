#ifndef _iman_h_
#define _iman_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <sys/time.h>

#define BUFFER_SIZE 8192

void printer(char *text, bool print, char *command);

int iman(char *command_name);

int iman_main(char *commands[], int counter, char *command, int *time);

#endif