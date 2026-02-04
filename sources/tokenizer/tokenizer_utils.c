/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:39 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 18:02:40 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_tokens(t_token_list *list);
void	reset_buf(t_buf *buf);
void	free_buf(t_buf *buf);
void			init_lexer_context(t_lexer_context *context);

void	reset_buf(t_buf *buf)
{
	buf->used_length = 0;
}

void	free_buf(t_buf *buf)
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

void	init_buffer(t_buf *buf)
{
	buf->characters = NULL;
	buf->quotes_map = NULL;
	buf->capacity = 0;
	buf->used_length = 0;
}

void	init_lexer_context(t_lexer_context *context)
{
	init_buffer(&context->buf);
	context->quote_mark = Q_NONE;
	context->i = 0;
	context->in_dq = false;
	context->in_sq = false;
}