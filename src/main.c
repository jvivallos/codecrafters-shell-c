#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //POSIX functions of dir and files
#include <unistd.h>

#include <errno.h>

#define DEBUG 0
#define LOGGER_LEVEL 0

int replace_new_line_null_terminator(char *buffer, size_t size)
{
  for (int i = 0; i < size; i++)
  {
    if (buffer[i] == '\n')
    {
      buffer[i] = '\0';
    }
  }

  return 0;
}

int is_exit_command(char *command, size_t size)
{
  // printf("strcmp %i", strcmp(command, "exit"));
  return strcmp(command, "exit 0") == 0;
}

void execute_echo(char *command, size_t size)
{
  char *text_start = command + 5; // jumps echo space
  printf("%s\n", text_start);
}

int check_for_command_old(char *folder, char *command)
{
  DIR *dp;
  struct dirent *entry;
  dp = opendir(folder);
  int found = 0;

  if (DEBUG)
  {
    printf("new check for folder %s and command %s\n", folder, command);
  }

  if (dp != NULL)
  {
    while ((entry = readdir(dp)) != NULL)
    {

      if (strncmp(command, entry->d_name, strlen(entry->d_name)) == 0)
      {
        found++;
      }
    }
    // puts(entry->d_name);
    (void)closedir(dp);
    if (found)
      return 1;
  }
  else
  {
    // perror("Couldn't open the directory");
    return 0;
  }

  return 0;
}

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

void execute_external(char *command, size_t size)
{
  jv_log("Executing external %s", command);

  char *external_command = extract_command(command);
  char *parameters = command + strlen(external_command) + 1;

  if (DEBUG)
  {
    printf("Program %s with parameters %s\n", external_command, parameters);
  }

  if (!validate_command_exists(external_command))
  {
    printf("%s: command not found\n", external_command);
  }
  else
  {
    system(command);
  }
}

void execute_pwd()
{
  puts(getcwd(NULL, 0));
}

void execute_cd(char *command)
{
  errno = 0;
  char *external_command = extract_command(command);
  char *parameters = command + strlen(external_command) + 1;

  chdir(parameters);
  if (errno == ENOENT)
  {
    printf("cd: %s: No such file or directory\n", parameters);
  }

  free(external_command);
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
}

int parse_command(char *command, size_t size)
{
  if (strncmp(command, "echo ", 5) == 0)
  {
    execute_echo(command, size);
    return 1;
  }
  else if (strncmp(command, "type ", 5) == 0)
  {
    return execute_type(command, size);
  }
  else if (is_builtin(command))
  {
    execute_builtin(command, size);
    return 1;
  }
  else
  {
    execute_external(command, size);
    return 1;
  }
  return 0;
}

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);
  set_logger(LOGGER_LEVEL);
  // Uncomment this block to pass the first stage

  // Wait for user input
  char input[150];
  while (1)
  {
    printf("$ ");
    fgets(input, 100, stdin);
    replace_new_line_null_terminator(input, 150);
    if (is_exit_command(input, 150))
    {
      return 0;
    }
    if (!parse_command(input, 150))
    {
      printf("%s: command not found\n", input);
    }
  }

  return 0;
}
