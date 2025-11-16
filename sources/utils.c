#include "minishell.h"

void free_tokens(t_token_list *list);

void free_tokens(t_token_list *list)
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