#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //POSIX functions of dir and files

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

int check_for_command(char *folder, char *command)
{
  DIR *dp;
  struct dirent *entry;
  dp = opendir(folder);

  if (dp != NULL)
  {
    while ((entry = readdir(dp)) != NULL)
    {

      if (strncmp(command, entry->d_name, strlen(entry->d_name)) == 0)
      {
        return 1;
      }
    }
    // puts(entry->d_name);

    (void)closedir(dp);
  }
  else
  {
    // perror("Couldn't open the directory");
    return 0;
  }

  return 0;
}

int execute_type(char *command, size_t size)
{
  char *command_start = command + 5;
  char buffer[size - 5];
  strncpy(buffer, command_start, size - 5);
  if (strncmp(command_start, "echo", 4) == 0 ||
      strncmp(command_start, "exit", 4) == 0 ||
      strncmp(command_start, "type", 4) == 0)
  {
    printf("%s is a shell builtin\n", buffer);
  }
  else
  {
    char *path = getenv("PATH");
    char *dir_token = strtok(path, ":");
    int found = 0;
    while (dir_token)
    {

      // puts(token);
      if (check_for_command(dir_token, command))
      {
        printf("%s is %s\n", command_start, dir_token);
        found++;
        break;
      }
      dir_token = strtok(NULL, ":");
    }

    if (!found)
      printf("%s: not found\n", command_start);
  }

  return 1;
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
  return 0;
}

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  // Uncomment this block to pass the first stage

  // Wait for user input
  char input[100];
  while (1)
  {
    printf("$ ");
    fgets(input, 100, stdin);
    replace_new_line_null_terminator(input, 100);
    if (is_exit_command(input, 100))
    {
      return 0;
    }
    if (!parse_command(input, 100))
    {
      printf("%s: command not found\n", input);
    }
  }

  return 0;
}
