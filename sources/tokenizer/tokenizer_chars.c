#include "minishell.h"

int boost_buf(t_buf *buf, size_t needed_length);

int append_char(char c, t_buf *buf, t_qmark quote_mark)
{
    if (!boost_buf(buf, buf->used_length + 1))
    {
        err_malloc_print("tokenizer: char append");
        return (0);
    }

    buf->characters[buf->used_length] = c;
    buf->quotes_map[buf->used_length] = quote_mark;
    buf->used_length++;
    return (1);
}

int boost_buf(t_buf *buf, size_t needed_length)
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
        return (0);
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
    return (1);
}