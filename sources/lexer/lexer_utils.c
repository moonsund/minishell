#include "minishell.h"

bool is_empty(const char *str);
bool is_space(unsigned char c);
bool is_operator(unsigned char c);
void append_token(t_token_list *list, t_token *token);
void free_tokens(t_token_list *list);
void reset_buf(t_buf *buf);
void free_buf(t_buf *buf);

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

void append_token(t_token_list *list, t_token *token)
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

void reset_buf(t_buf *buf)
{
    buf->used_length = 0;
}

void free_buf(t_buf *buf)
{
    if (buf->characters)
        free(buf->characters);
    if (buf->quotes_map)
        free(buf->quotes_map);
    buf->characters = NULL;
    buf->quotes_map = NULL;
    buf->capacity = 0;
    buf->used_length = 0;
}

void init_buffer(t_buf *buf)
{
    buf->characters = NULL;
    buf->quotes_map = NULL;
    buf->capacity = 0;
    buf->used_length = 0;
}