#ifndef _LOGGER_H_
#define _LOGGER_H_

void set_logger(int level);

void jv_log(char *format, ...);
void jv_debug(const char *format, ...);
void jv_error(const char *format, ...);

#endif