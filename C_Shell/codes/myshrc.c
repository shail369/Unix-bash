#include "myshrc.h"

char *trim_whitespace(char *str)
{
    char *end;
    while (isspace((unsigned char)*str))
    {
        str++;
    }
    if (*str == 0)
    {
        return str;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
    {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}

void aliases()
{
    FILE *file;
    char buffer[MAX_LENGTH];
    int line_count = 0;
    file = fopen(".myshrc", "r");

    if (file == NULL)
    {
        perror("Unable to open file");
        return;
    }
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        alias[i] = malloc(MAX_LENGTH);
        alias_function[i] = malloc(MAX_LENGTH);
        if (alias[i] == NULL)
        {
            perror("Unable to allocate memory");
            break;
        }
    }
    while (fgets(buffer, MAX_LENGTH, file) != NULL)
    {
        char *temp = strtok(buffer, "=");
        strcpy(alias[line_count], trim_whitespace(temp));
        temp = strtok(NULL, "=");
        strcpy(alias_function[line_count], trim_whitespace(temp));
        line_count++;
    }
    fclose(file);
    return;
}

int alias_replacement(int index, char *commands[], int *counter)
{
    int num_replacement = 0;
    char copy[MAX_LENGTH];
    strcpy(copy, alias_function[index]);
    char *temp = strtok(copy, " ");
    char *temp_store[MAX_PROCESSES];
    while (temp)
    {
        temp_store[num_replacement] = malloc(MAX_LENGTH);
        strcpy(temp_store[num_replacement++], temp);
        temp = strtok(NULL, " ");
    }
    for (int i = *counter - 1; i > 0; i--)
    {
        strcpy(commands[i + num_replacement - 1], commands[i]);
    }
    for (int i = 0; i < num_replacement; i++)
    {
        strcpy(commands[i], temp_store[i]);
    }
    *counter += num_replacement - 1;
    return 0;
}