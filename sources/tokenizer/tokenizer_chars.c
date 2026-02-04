/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_chars.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:32 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 18:22:55 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int				append_char(char c, t_buf *buf, t_qmark quote_mark);
int				boost_buf(t_buf *buf, size_t needed_length);
static void		replace_buffers(t_buf *buf, char *new_chars, t_qmark *new_qmap,
					size_t new_capacity);
static int		alloc_new_buffers(size_t capacity, char **new_chars,
					t_qmark **new_qmap);
static size_t	calc_new_capacity(size_t current, size_t needed);

int	append_char(char c, t_buf *buf, t_qmark quote_mark)
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

int	boost_buf(t_buf *buf, size_t needed_length)
{
	size_t	new_capacity;
	char	*new_characters;
	t_qmark	*new_quotes_map;

	if (buf->capacity >= needed_length)
		return (1);
	new_capacity = calc_new_capacity(buf->capacity, needed_length);
	if (!alloc_new_buffers(new_capacity, &new_characters, &new_quotes_map))
		return (0);
	replace_buffers(buf, new_characters, new_quotes_map, new_capacity);
	return (1);
}

static void	replace_buffers(t_buf *buf, char *new_chars, t_qmark *new_qmap,
		size_t new_capacity)
{
	if (buf->characters)
		ft_memcpy(new_chars, buf->characters, buf->used_length);
	if (buf->quotes_map)
		ft_memcpy(new_qmap, buf->quotes_map, buf->used_length
			* sizeof(*new_qmap));
	free(buf->characters);
	free(buf->quotes_map);
	buf->characters = new_chars;
	buf->quotes_map = new_qmap;
	buf->capacity = new_capacity;
}

static int	alloc_new_buffers(size_t capacity, char **new_chars,
		t_qmark **new_qmap)
{
	*new_chars = malloc(capacity * sizeof(**new_chars));
	*new_qmap = malloc(capacity * sizeof(**new_qmap));
	if (!*new_chars || !*new_qmap)
	{
		free(*new_chars);
		free(*new_qmap);
		return (0);
	}
	return (1);
}

static size_t	calc_new_capacity(size_t current, size_t needed)
{
	size_t	capacity;

	if (current > 0)
		capacity = current;
	else
		capacity = 16;
	while (capacity < needed)
		capacity *= 2;
	return (capacity);
}
