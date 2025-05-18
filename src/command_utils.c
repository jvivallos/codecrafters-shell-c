#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //POSIX functions of dir and files
#include <unistd.h>

#include <errno.h>

#include "logger.h"

#define DEBUG 0

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
    int command_end_position = strcspn(command, " ");
    char *external_command = malloc(command_end_position * sizeof(char));
    external_command[command_end_position] = '\0';
    strncpy(external_command, command, command_end_position);

    jv_log("First space found at %i \n", command_end_position);

    return external_command;
}