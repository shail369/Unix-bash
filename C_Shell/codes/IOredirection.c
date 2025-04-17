#include "IOredirection.h"
#include "helper.h"

int proper(char *commands[], int counter, bool *input_file, bool *output_file_write, bool *output_file_append, char *input_filename, char *output_filename)
{
    if (!strcmp(commands[counter - 1], ">") || !strcmp(commands[counter - 1], "<") || !strcmp(commands[counter - 1], ">>"))
    {
        printf("Invalid Command\n");
        return 1;
    }
    for (int i = 0; i < counter - 1; i++)
    {
        if (!strcmp(commands[i], ">"))
        {
            if (*output_file_append || *output_file_write)
            {
                printf("Multiple Output files\n");
                return 1;
            }
            if (i && !strcmp(commands[i - 1], "<"))
            {
                printf("Invalid Command\n");
                return 1;
            }
            *output_file_write = true;
            strcpy(output_filename, commands[i + 1]);
        }
        if (!strcmp(commands[i], ">>"))
        {
            if (*output_file_append || *output_file_write)
            {
                printf("Multiple Output files\n");
                return 1;
            }
            if (i && !strcmp(commands[i - 1], "<"))
            {
                printf("Invalid Command\n");
                return 1;
            }
            *output_file_append = true;
            strcpy(output_filename, commands[i + 1]);
        }
        if (!strcmp(commands[i], "<"))
        {
            if (*input_file)
            {
                printf("Multiple Input files\n");
                return 1;
            }
            if (i && !strcmp(commands[i - 1], "<") || !strcmp(commands[i - 1], "<<"))
            {
                printf("Invalid Command\n");
                return 1;
            }
            *input_file = true;
            strcpy(input_filename, commands[i + 1]);
        }
    }
    return 0;
}

