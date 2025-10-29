#include "minishell.h"

static t_token *create_token(char *str, t_token_type type);
static void add_token(t_token_list *list, t_token *new_token);
void clean(t_token_list *list);

bool tokenize(const char *str, t_token_list *list)
{
    size_t i;
    unsigned int start;
    size_t len;
    char *chunk;
    t_token *new_token;
    t_token_type type;

    i = 0;
    while (str[i] != '\0')
    {
        start = i;
        while (str[i] && !is_space(str[i]))
            i++;
        len = i - start;
        if (str[start] == '|' && (len == 1))
        {
            chunk = ft_substr(str, start, len);
            if (!chunk)
                return (clean(list), false);
            type = PIPE;
        }
        else if ((str[start] == '<') && (len == 1))
        {
            chunk = ft_substr(str, start, len);
            if (!chunk)
                return (clean(list), false);
            type = REDIR_IN;
        }
        else if ((str[start] == '<') && (len == 2)&& (str[start + 1] == '<'))
        {
            chunk = ft_substr(str, start, len);
            if (!chunk)
                return (clean(list), false);
            type = HEREDOC;
        }
        else if ((str[start] == '>') && (len == 1))
        {
            chunk = ft_substr(str, start, len);
            if (!chunk)
                return (clean(list), false);
            type = REDIR_OUT;
        }
        else if ((str[start] == '>') && (len == 2) && (str[start + 1] == '>'))
        {
            chunk = ft_substr(str, start, len);
            if (!chunk)
                return (clean(list), false);
            type = APPEND;
        }
        else
        {
            chunk = ft_substr(str, start, len);
            if (!chunk)
                return (clean(list), false);
            type = WORD;
        }
        new_token = create_token(chunk, type);
        if (!new_token)
            return (free(chunk), clean(list), false);
        add_token(list, new_token);
        i++;
    }
    return (true);
}

// find_chunk(size_t *i, const char *str)
// {
//     unsigned int start;
//     size_t len;
//     char *chunk;

//     start = i;
//     while(str[*i] && !is_space(str[*i]))
//     {
        

        

//     }

// }


// detect_token_type()
// {
    


// }




static t_token *create_token(char *str, t_token_type type)
{
     t_token *new_token;

     new_token = malloc(sizeof(t_token));
     if (!new_token)
        return (NULL);
     new_token->text = str;
     new_token->type = type;
     new_token->next = NULL;
     return (new_token);
}

static void add_token(t_token_list *list, t_token *new_token)
{
    t_token *last_token;

    if (!list || !new_token)
        return ;
    if (list->head == NULL)
    {
        list->head = new_token;
        list->count++;
        return ;
    }
    last_token = list->head;
    while (last_token->next != NULL)
        last_token = last_token->next;
    last_token->next = new_token;
    list->count++;
}

void clean(t_token_list *list)
{
    (void)list;
}