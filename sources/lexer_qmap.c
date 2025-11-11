#include "minishell.h"

static t_token *make_operator_token(t_token_type type, const char *literal);
static void append_token(t_token_list *list, t_token *token);
static int append_char(t_buf *buf, char c, t_qmark quote_mark);
static int boost_buf(t_buf *buf, size_t needed_length);
static void clean_buf(t_buf *buf);
static void free_buf(t_buf *buf);
static t_token *make_word_token(t_buf *buf);

bool tokenize_with_qmap(const char *str, t_token_list *tokens)
{
    size_t i;
    bool in_sq;
    bool in_dq;
    t_qmark quote_mark;
    t_buf buf = {0}; // buffer initialisation needed
    t_token *token;

    i = 0;
    in_sq = false;
    in_dq = false;
    quote_mark = Q_NONE;

    while (str[i] != '\0') 
    {

        // skip spaces at the beginning of the line and process operators
        if (!in_sq && !in_dq)
        {
            if (buf.used_length == 0)
            {
                while (is_space((unsigned char)str[i]))
                    i++;
                if (!str[i])
                    break;
            }
        }

        // process quote scenarios
        if (!in_dq && str[i] == '\'')
        {
            in_sq = !in_sq;
            if (in_sq)
                quote_mark = Q_SQ;
            else
                quote_mark = Q_NONE;
            i++;
            continue;
        }

        if (!in_sq && str[i] == '\"')
        {
            in_dq = !in_dq;
            if (in_dq)
                quote_mark = Q_DQ;
            else
                quote_mark = Q_NONE;
            i++;
            continue;
        } 

        // process white spaces outside quotes
        if (!in_sq && !in_dq && is_space((unsigned char)str[i]))
        {
            if (buf.used_length > 0)
            {
                token = make_word_token(&buf);
                if (!token)
                {
                    free_buf(&buf);
                    return 0;
                }
                append_token(tokens, token);
                clean_buf(&buf);
            }
            i++;
            continue;
        }

        // process operators outside quotes
        if (!in_sq && !in_dq && is_operator(str[i]))
    {
        if (buf.used_length > 0)
        {
            token = make_word_token(&buf);
            if (!token)
            {
                free_buf(&buf);
                return 0;
            }
            append_token(tokens, token);
            clean_buf(&buf);
        }

        if (str[i] == '|') 
        {
            append_token(tokens, make_operator_token(TOK_PIPE, "|"));
            i++;
        }
        else if (str[i] == '<')
        {
            if (str[i+1] == '<')
            {
                append_token(tokens, make_operator_token(TOK_HEREDOC, "<<"));
                i += 2;
            }
            else
            {
                append_token(tokens, make_operator_token(TOK_REDIR_IN, "<"));
                i++;
            }
        }
        else if (str[i] == '>')
        {
            if (str[i+1] == '>')
            {
                append_token(tokens, make_operator_token(TOK_APPEND, ">>"));
                i += 2;
            }
            else
            {
                append_token(tokens, make_operator_token(TOK_REDIR_OUT, ">"));
                i++;
            }
        }
        continue;
    }

        // accumulate characters in the buffer
        if (!append_char(&buf, str[i], quote_mark))
        {
            free_buf(&buf);
            return (0);
        }
        i++;
    }

    if (in_sq || in_dq)
    {

        free_buf(&buf);
        return (0);
    }

    if (buf.used_length > 0)
    {
        token = make_word_token(&buf);
        if (!token)
        {
            free_buf(&buf);
            return (0);
        }
        append_token(tokens, token);
    }

    free_buf(&buf);
    return (true);
}


static t_token *make_operator_token(t_token_type type, const char *literal)
{
    t_token *token;
    size_t len;

    token = malloc(sizeof(*token));
    if (!token)
        return (NULL);

    len = ft_strlen(literal);
    token->raw_str = malloc(sizeof(char) * (len + 1));
    if (!token->raw_str)
    {
        free(token);
        return (NULL);
    }

    ft_memcpy(token->raw_str, literal, len + 1);
    token->length = len;
    token->type = type;
    token->quotes_map = NULL;
    token->next = NULL;
    return (token);
}

static void append_token(t_token_list *list, t_token *token)
{
    if (!token)
        return ;
    if (list->head == NULL)
    {
        list->tail = token;
        list->head = token;
    }
    else
    {
        list->tail->next = token;
        list->tail = token;
    }
    list->count++;
}

static int append_char(t_buf *buf, char c, t_qmark quote_mark)
{
    if (!boost_buf(buf, buf->used_length + 1))
        return (0);

    buf->characters[buf->used_length] = c;
    buf->quotes_map[buf->used_length] = quote_mark;
    buf->used_length++;
    return (1);
}

static int boost_buf(t_buf *buf, size_t needed_length)
{
    size_t   new_capacity;
    char    *new_characters;
    t_qmark *new_quotes_map;

    if (buf->capacity >= needed_length)
        return (1);

    if (buf->capacity)
        new_capacity = buf->capacity;
    else
        new_capacity = 16;

    while (new_capacity < needed_length)
        new_capacity *= 2;

    new_characters = malloc(new_capacity * sizeof(*new_characters));
    new_quotes_map = malloc(new_capacity * sizeof(*new_quotes_map));
    if (!new_characters || !new_quotes_map)
    {
        free(new_characters);
        free(new_quotes_map);
        return 0;
    }

    if (buf->characters)
        ft_memcpy(new_characters, buf->characters, buf->used_length);
    if (buf->quotes_map)
        ft_memcpy(new_quotes_map, buf->quotes_map,
                  buf->used_length * sizeof(*new_quotes_map));

    free(buf->characters);
    free(buf->quotes_map);

    buf->characters = new_characters;
    buf->quotes_map = new_quotes_map;
    buf->capacity = new_capacity;
    return 1;
}


static void clean_buf(t_buf *buf)
{
    buf->used_length = 0;
}

static void free_buf(t_buf *buf)
{
    free(buf->characters);
    free(buf->quotes_map);
    buf->characters = NULL;
    buf->quotes_map = NULL;
    buf->capacity = 0;
    buf->used_length = 0;

}

static t_token *make_word_token(t_buf *buf)
{
    t_token *token;

    token = malloc(sizeof(*token));
    if (!token)
        return (NULL);

    token->raw_str = malloc(sizeof(char) * (buf->used_length + 1));
    token->quotes_map = malloc(sizeof(t_qmark) * buf->used_length);
    if (!token->raw_str || !token->quotes_map)
    {
        free(token->raw_str);
        free(token->quotes_map);
        free(token);
        return (NULL);
    }

    if (buf->capacity) 
    {
        ft_memcpy(token->raw_str, buf->characters, buf->used_length);
        ft_memcpy(token->quotes_map, buf->quotes_map,
                buf->used_length * sizeof(*buf->quotes_map));
    }

    token->raw_str[buf->used_length] = '\0';
    token->length = buf->used_length;
    token->type = TOK_WORD;
    token->next = NULL;
    return (token);
}
