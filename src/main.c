#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  // Uncomment this block to pass the first stage
  printf("$ ");

  // Wait for user input
  char input[100];
  while (input[0] != 'q')
  {
    fgets(input, 100, stdin);
    replace_new_line_null_terminator(input, 100);
    printf("%s: command not found\n", input);
  }

  return 0;
}
