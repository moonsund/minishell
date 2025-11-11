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

bool is_operator(unsigned char c)
{
    return (c == '<' || c == '>' || c == '|');
}


void clean(t_token_list *list)
{
    t_token *cur;
    t_token *next;

    if (!list || list->head == NULL)
        return ;

    cur = list->head;
    while (cur != NULL)
    {
        next = cur->next;
        free(cur->raw_str);
        free(cur);
        cur = next;
    }
    list->head = NULL;
    list->count = 0;
}