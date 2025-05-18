#ifndef _COMMAND_UTILS_H_
#define _COMMAND_UTILS_H_

int validate_command_exists(char *command);
int check_for_command(char *folder, char *command);
char *extract_command(char *command);

#endif