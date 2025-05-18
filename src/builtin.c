#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //POSIX functions of dir and files
#include <unistd.h>
#include <ctype.h>

#include <errno.h>

#include "logger.h"
#include "command_utils.h"

#define DEBUG 0

int is_builtin(char *command)
{
    return strncmp(command, "cd", 2) == 0 ||
           strncmp(command, "pwd", 3) == 0 ||
           strncmp(command, "echo", 4) == 0 ||
           strncmp(command, "exit", 4) == 0 ||
           strncmp(command, "type", 4) == 0;
}

int execute_type(char *command, size_t size)
{
    char *command_start = command + 5;
    char buffer[size - 5];
    strncpy(buffer, command_start, size - 5);
    if (is_builtin(command_start))
    {
        printf("%s is a shell builtin\n", buffer);
    }
    else
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

            // puts(token);
            if (check_for_command(dir_token, command_start))
            {
                printf("%s is %s/%s\n", command_start, dir_token, command_start);
                found++;
                break;
            }
            dir_token = strtok(NULL, ":"); // null because the original string is stored in a static variable
        }

        if (!found)
            printf("%s: not found\n", command_start);

        free(path);
    }

    return 1;
}

void execute_pwd()
{
    puts(getcwd(NULL, 0));
}

int search_tilda(char *string)
{
    size_t length = strlen(string);
    for (int i = 0; i < length; i++)
    {
        if (string[i] == '~')
        {
            return i;
        }
    }

    return -1;
}

void execute_cd(char *command)
{
    errno = 0;
    char *external_command = extract_command(command);
    char *parameters = command + strlen(external_command) + 1;

    // Parameters is null terminated
    int tilda_pos = search_tilda(parameters);

    if (tilda_pos > -1)
    {
        char *home_dir = getenv("HOME");
        int home_length = strlen(home_dir);
        char parameters_with_home[strlen(home_dir) + strlen(parameters) + 1];
        snprintf(parameters_with_home, sizeof(parameters_with_home), "%s%s", home_dir, parameters + tilda_pos + 1);
        chdir(parameters_with_home);
    }
    else
    {
        chdir(parameters);
    }

    if (errno == ENOENT)
    {
        printf("cd: %s: No such file or directory\n", parameters);
    }

    free(external_command);
}

char *remove_any_quotes(char *original)
{
    int original_lenght = strlen(original);
    char *clean_string = malloc((original_lenght + 1) * sizeof(char));
    char *buff = original;

    int i = 0;

    while (buff[0] != '\0')
    {

        jv_log("Char %c", buff[0]);

        // Handle scape character
        if (buff[0] == '\\')
        {
            jv_log("Entering scape char");
            buff++;                    // skip scape char
            clean_string[i] = buff[0]; // copy scaped
            buff++;
            i++;
            continue;
        }

        // Handle quotes
        if (buff[0] == '\'' || buff[0] == '"')
        {
            char quote = buff[0];
            jv_log("Entering Quote %c", buff[0]);
            buff++;
            while (buff[0] != quote)
            {
                jv_log("Char %c", buff[0]);
                clean_string[i] = buff[0];
                i++;
                buff++;
            }
            jv_log("Exiting with %c", buff[0]);
            buff++;
            jv_log("Next char is %c", buff[0]);
        }

        // Handle spaces outside quotes
        if (isspace(buff[0]))
        {
            jv_log("Entering space", buff[0]);
            clean_string[i] = buff[0];
            i++;
            buff++;
            while (isspace(buff[0]))
            {
                jv_log("space", buff[0]);
                buff++;
            }
            jv_log("Exiting space with %c", buff[0]);
        }

        if (buff[0] == '\'' || buff[0] == '"')
            continue;

        clean_string[i] = buff[0];
        i++;
        buff++;
    }
    clean_string[i + 1] = '\0';

    return clean_string;
}

void execute_echo(char *command)
{
    char *external_command = extract_command(command);
    char *parameters = command + strlen(external_command) + 1;
    printf("%s\n", remove_any_quotes(parameters));
}

void execute_builtin(char *command, size_t size)
{
    jv_log("Executing builtin %s", command);
    if (strncmp(command, "pwd", 3) == 0)
    {
        execute_pwd();
    }
    else if (strncmp(command, "cd", 2) == 0)
    {
        execute_cd(command);
    }
    else if (strncmp(command, "echo", 4) == 0)
    {
        execute_echo(command);
    }
}
