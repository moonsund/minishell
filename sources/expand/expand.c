#include "minishell.h"

static bool token_needs_expansion(t_token *token);
static int expand_word_token(t_token *token, t_env_var_list *env_vars);
static char *get_last_status_string();
static int append_str(char *str, t_buf *buf, t_qmark quote_mark);

int expand_tokens(t_token_list *tokens, t_env_var_list *env_vars)
{    
    t_token *prev;
    t_token *cur;
    
    if (!tokens)
        return (0);
    
    prev = NULL;
    cur = tokens->head;
    while (cur)
    {
        if (cur->type == TOK_WORD)
        {
            if (!prev || prev->type != TOK_HEREDOC)
                expand_word_token(cur, env_vars);
        }
            prev = cur;
            cur = cur->next;
    }
    return (1);
}

static int expand_word_token(t_token *token, t_env_var_list *env_vars)
{
    t_buf buf;
    size_t i;
    size_t j;
    size_t start;
    char *val;
    size_t var_length;
    char *var_name;

    if (!token || token->type != TOK_WORD || !token->quotes_map || !token->raw_str)
        return (0);
    
    if (!token_needs_expansion(token))
        return (1);

    init_buffer(&buf);

    i = 0;
    while (i < token->length)
    {
        start = 0;

        if (token->raw_str[i] == '$')
        {
            start = i + 1;
            if (start >= token->length)
            {
                if (!append_char(token->raw_str[i], &buf, Q_NONE))
                {
                    free_buf(&buf);
                    return (0);
                }
                i++;
                continue;
            }

            if (token->raw_str[start] == '?')
            {
                val = get_last_status_string();
                if (!val || !append_str(val, &buf, Q_NONE))
                {
                    free_buf(&buf);
                    return (0);
                }
                i += 2;
                continue;
            }

            j = start;
            while (j < token->length && (ft_isalnum((unsigned char)token->raw_str[j]) 
                    || token->raw_str[j] == '_'))
                j++;

            if (j == start)
            {
                if (!append_char(token->raw_str[i], &buf, Q_NONE))
                {
                    free_buf(&buf);
                    return (0);
                }
                i++;
                continue;
            }

            var_length = j - start;

            var_name = (char *)malloc(sizeof(char) * (var_length + 1));
            if (!var_name)
                return (0);
            
            ft_memcpy(var_name, token->raw_str + start, var_length);
            var_name[var_length] = '\0';

            val = get_var_value(env_vars, var_name);
            free(var_name);

            if (val && val[0] != '\0')
            {
                if (!append_str(val, &buf, Q_NONE))
                {
                    free_buf(&buf);
                    return (0);
                }                
            }
            i = j;
            continue;
        }

        else
        {
            if (!append_char(token->raw_str[i], &buf, Q_NONE))
            {
                free_buf(&buf);
                return (0);
            }
        }
        i++;
    }
    append_char('\0', &buf, Q_NONE);

    free(token->raw_str);
    free(token->quotes_map);

    token->quotes_map = buf.quotes_map;
    token->raw_str = buf.characters;
    token->length = buf.used_length - 1;
    return (1);
}

static int append_str(char *str, t_buf *buf, t_qmark quote_mark)
{
    size_t i;
    size_t str_length;

    str_length = ft_strlen(str);

    if (!boost_buf(buf, buf->used_length + str_length))
        return (0);

    i = 0;
    while (i < str_length)
    {
        buf->characters[buf->used_length] = str[i];
        buf->quotes_map[buf->used_length] = quote_mark;
        buf->used_length++;
        i++;
    }
    return (1);
}


static char *get_last_status_string()  // using global variable?
{
    return NULL;
}

static bool token_needs_expansion(t_token *token)
{
    size_t i;
    const char *str;
    t_qmark *q_map;

    str = token->raw_str;
    q_map = token->quotes_map;

    i = 0;
    while (i < token->length)
    {
        if (str[i] == '$' && q_map[i] != Q_SQ)
            return (true);
    i++;
    }
    return (false);
}