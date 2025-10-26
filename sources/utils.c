#include "minishell.h"

bool is_empty(const char *str);
bool is_space(unsigned char c);

bool is_empty(const char *str)
{
    if (!str || !*str)
        return (true);
    while (*str)
    {
        if (!is_space((unsigned char)*str))
            return (false);
        str++;
    }
    return (true);
}

bool is_space(unsigned char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}