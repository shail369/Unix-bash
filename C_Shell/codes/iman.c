#include "iman.h"

void printer(char *text, bool print, char *command)
{
    char *src = text;
    int in_tag = 0;
    while (*src)
    {
        if (!strncmp(src, command, strlen(command)))
        {
            print = true;
        }
        if (*src == '<')
        {
            in_tag = 1;
        }
        else if (*src == '>')
        {
            in_tag = 0;
            src++;
            continue;
        }

        if (!in_tag && print)
        {
            printf("%c", *src);
        }
        src++;
    }
}

int iman(char *command_name)
{
    int sock;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char request[BUFFER_SIZE], response[BUFFER_SIZE];
    int bytes_received;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    server = gethostbyname("man.he.net");
    if (server == NULL)
    {
        fprintf(stderr, "Host not found\n");
        close(sock);
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    server_addr.sin_port = htons(80);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: man.he.net\r\n"
             "Connection: close\r\n\r\n",
             command_name);

    if (send(sock, request, strlen(request), 0) < 0)
    {
        perror("Send failed");
        close(sock);
        return 1;
    }
    bool print = false;

    while ((bytes_received = recv(sock, response, sizeof(response) - 1, 0)) > 0)
    {
        response[bytes_received] = '\0';
        printer(response, print, command_name);
        print = true;
    }

    if (bytes_received < 0)
    {
        perror("Receive failed");
        close(sock);
        return 1;
    }
    close(sock);
    return 0;
}

int iman_main(char *commands[], int counter, char *command, int *time)
{
    if (counter < 2)
    {
        printf("Invalid Commands\n");
        return 1;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    char *iman_page = commands[1];
    if (iman(iman_page))
    {
        gettimeofday(&end, NULL);
        int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        if (time_taken >= 2)
        {
            strcpy(command, commands[0]);
            *time = time_taken;
        }
        return 1;
    }
    gettimeofday(&end, NULL);
    int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    if (time_taken >= 2)
    {
        strcpy(command, commands[0]);
        *time = time_taken;
    }
    return 0;
}
