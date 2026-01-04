#include "minishell.h"

static bool token_needs_expansion(t_token *token);
static t_exit_status expand_word_token(t_token *token, t_env_var_list *env_vars, t_exit_status exit_status);
char *get_last_status_string(t_exit_status exit_status);
static int append_str(t_buf *buf, char *str, t_qmark quote_mark);

t_exit_status expand_tokens(t_token_list *tokens, t_env_var_list *env_vars, t_exit_status exit_status)
{    
    t_token *prev;
    t_token *cur;
    t_exit_status new_exit_status;
    
    if (!tokens)
        return (ES_GENERAL);
    
    prev = NULL;
    cur = tokens->head;
    while (cur)
    {
        if (cur->type == TOK_WORD)
        {
            if (!prev || prev->type != TOK_HEREDOC)
            {
                new_exit_status = expand_word_token(cur, env_vars, exit_status);
                if (new_exit_status != ES_SUCCESS)
                    return (new_exit_status);
            }
        }
            prev = cur;
            cur = cur->next;
    }
    return (ES_SUCCESS);
}

static t_exit_status expand_word_token(t_token *token, t_env_var_list *env_vars, t_exit_status exit_status)
{
    t_buf buf;
    size_t i;
    size_t j;
    size_t start;
    char *val;
    size_t var_length;
    char *var_name;

    if (!token || token->type != TOK_WORD || !token->quotes_map || !token->raw_str)
        return (ES_GENERAL);
    
    if (!token_needs_expansion(token))
        return (ES_SUCCESS);

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
                    return (ES_GENERAL);
                }
                i++;
                continue;
            }

            if (token->raw_str[start] == '?')
            {
                val = get_last_status_string(exit_status);
                if (!val || !append_str(&buf, val, Q_NONE)) // (&buf, val, Q_NONE)
                {
                    free_buf(&buf);
                    return (ES_GENERAL);
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
                    return (ES_GENERAL);
                }
                i++;
                continue;
            }

            var_length = j - start;
            var_name = (char *)malloc(sizeof(char) * (var_length + 1));
            if (!var_name)
                return (ES_GENERAL);
            
            ft_memcpy(var_name, token->raw_str + start, var_length);
            var_name[var_length] = '\0';

            val = get_var_value(env_vars, var_name);
            free(var_name);

            if (val && val[0] != '\0')
            {
                if (!append_str(&buf, val, Q_NONE))
                {
                    free_buf(&buf);
                    return (ES_GENERAL);
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
                return (ES_GENERAL);
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
    return (ES_SUCCESS);
}

static int append_str(t_buf *buf, char *str, t_qmark quote_mark)
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

char *get_last_status_string(t_exit_status exit_status)
{
    return (ft_itoa(exit_status));
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