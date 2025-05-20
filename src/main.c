#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //POSIX functions of dir and files
#include <unistd.h>
#include <sys/wait.h>

#include <errno.h>

#include "logger.h"
#include "builtin.h"
#include "command_utils.h"

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

void exec_command(char *argv[])
{
  pid_t pid = fork();
  if (pid == 0)
  {
    jv_log("%s", argv[0]);
    execvp(argv[0], argv);
    perror("execv");
    exit(1);
  }
  else if (pid < 0)
    perror("Error forking child process.");
  else
  {
    int status;
    waitpid(pid, &status, 0);
  }
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
    char *argv[40];
    int status = extract_parameters(command, argv);

    // system(command);
    exec_command(argv);
  }
}

int parse_command(char *command, size_t size)
{
  if (strncmp(command, "type ", 5) == 0)
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
