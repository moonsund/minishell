#include "minishell.h"

static t_token *create_token(char *str, t_token_type type);
static void add_token(t_token_list *tokens, t_token *new_token);
static bool read_word_until_break(const char *str, size_t *i);


bool tokenize(const char *str, t_token_list *tokens)
{
    size_t start;
    size_t i;
    t_token_type type;
    size_t len;
    char *chunk;
    t_token *new_token;

    i = 0;
    while (str[i] != '\0') 
    {
        if (is_space(str[i])) 
        {
            i++;
            continue;
        }

        start = i;
        if (str[i] == '|') 
        {
            type = PIPE;
            i++;
        }
        
        else if (str[i] == '<')
        {
            if (str[i+1] == '<')
            {
                type = HEREDOC;
                i += 2;
            }
            else
            {
                type = REDIR_IN;
                i++;
            }
        }
        else if (str[i] == '>')
        {
            if (str[i+1] == '>')
            {
                type = APPEND;
                i += 2;
            }
            else
            {
                type = REDIR_OUT;
                i++;
            }
        }
        else
        {
            if (!read_word_until_break(str, &i))
                return (false); 
            type = WORD;
        }

        len = i - start;
        chunk = ft_substr(str, start, len);
        if (!chunk)
            return (clean(tokens), false);

        new_token = create_token(chunk, type);
        if (!new_token)
            return (free(chunk), clean(tokens), false);

        add_token(tokens, new_token);
    }
    return true;
}

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

static bool read_word_until_break(const char *str, size_t *i)
{
    while (str[*i] && !is_space(str[*i]) && !is_operator(str[*i])) 
    {
        if (str[*i] == '\'') 
        {
            (*i)++;
            while (str[*i] && str[*i] != '\'')
                (*i)++;
            if (str[*i] != '\'') 
                return (false);
            (*i)++;
        } 
        else if (str[*i] == '"') 
        {
            (*i)++;
            while (str[*i] && str[*i] != '"')
                (*i)++;
            if (str[*i] != '"')
                return (false);
            (*i)++;
        }
        else
            (*i)++;
    }
    return (true);
}
