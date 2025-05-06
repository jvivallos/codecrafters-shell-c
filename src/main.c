#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int parse_command(char *command, size_t size)
{
  if (strncmp(command, "echo ", 5) == 0)
  {
    execute_echo(command, size);
    return 1;
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
