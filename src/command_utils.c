#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //POSIX functions of dir and files
#include <unistd.h>

#include <errno.h>

#include "logger.h"

#define DEBUG 0

#define MAX_INPUT_PARAMETERS 10

int check_for_command(char *folder, char *command)
{

    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", folder, command);

    if (DEBUG)
    {
        printf("new check for folder %s and command %s\n", full_path, command);
    }

    if (access(full_path, X_OK) == 0)
    {
        return 1;
    }

    return 0;
}

int validate_command_exists(char *command)
{
    char *path = strdup(getenv("PATH"));

    if (DEBUG)
    {
        puts(path);
    }
    char *dir_token = strtok(path, ":");
    int found = 0;

    while (dir_token)
    {
        if (check_for_command(dir_token, command))
        {
            free(path);
            return 1;
        }
        dir_token = strtok(NULL, ":"); // null because the original string is stored in a static variable
    }

    free(path);

    return 0;
}

char *extract_command(char *command)
{
    int command_end_position = -1;
    int quote_space_jump = 0;

    if (command[0] == '\'' || command[0] == '"')
    {
        char quote = command[0];
        char *search = command + 1;
        int i = 0; // it starts in one to compensate the +1 before
        while (search[0] != quote)
        {
            search++;
            i++;
        }
        quote_space_jump = 1;
        command_end_position = i; // this one is to copy the quote not skip it
    }
    else
    {
        command_end_position = strcspn(command, " ");
    }
    jv_log("End of command at %i", command_end_position);
    char *external_command = malloc(command_end_position * sizeof(char));
    external_command[command_end_position] = '\0';
    strncpy(external_command, command + quote_space_jump, command_end_position);

    jv_log("Command is %s", external_command);
    return external_command;
}

int extract_parameters(char *command, char *argv[])
{
    char *path = strdup(command);
    int i = 0;

    char *token = strtok(command, " ");
    while (token != NULL)
    {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }

    argv[i] = NULL; // Null-terminate the argument list

    free(path);

    return 1;
}