#include "minishell.h"

static size_t strlen_no_extra_spaces(const char *s);

char *normalize_str(const char *str)
{
    size_t i;
    size_t j;
    bool skip_spaces;
    size_t length;
    char *new_str;

    i = 0;
    j = 0;
    skip_spaces = true;
    length = strlen_no_extra_spaces(str);
    new_str = malloc(sizeof(char) * (length +1));
    if (!new_str)
        return (NULL);
    while (j < length)
    {
        if (is_space(str[i]))
        {
            if (!skip_spaces)
            {
                new_str[j] = str[i];
                skip_spaces = true;
                j++;
            }
        }
        else
        {
            new_str[j] = str[i];
            skip_spaces = false;
            j++;
        }
        i++;
    }   
    new_str[j] = '\0';
    return (new_str);
}

static size_t strlen_no_extra_spaces(const char *s)
{
    size_t i;
    size_t len;
    bool skip_spaces;

    i = 0;
    len = 0;
    skip_spaces = true;
    while (s[i])
    {
        if (is_space((unsigned char)s[i]))
        {
            if (!skip_spaces) 
            {
                len++;
                skip_spaces = true;
            }
        }
        else
        {
            len++;
            skip_spaces = false;
        }
        i++;
    }
    if (skip_spaces && len > 0)
        len--;
    return (len);
}