#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static int level = 0;

void set_logger(int new_level)
{
    level = new_level;
}

void jv_log(const char *format, ...)
{
    va_list arglist;
    if (level == 1)
    {
        va_start(arglist, format);
        vprintf(format, arglist);
        printf("\n");
        va_end(arglist);
    }
}

void jv_debug(const char *format, ...)
{
    va_list arglist;
    if (level == 2)
    {
        va_start(arglist, format);
        vprintf(format, arglist);

        va_end(arglist);
    }
}