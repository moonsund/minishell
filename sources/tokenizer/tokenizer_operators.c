/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_operators.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:36 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 17:49:15 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int				process_operator_token(t_token_type type, const char *literal,
					t_token_list *tokens, t_lexer_context *ctx);
static t_token	*make_operator_token(t_token_type type, const char *literal);

int	check_operators(const char *str, t_token_list *tokens, t_lexer_context *ctx)
{
	int	ret;

	if (!should_handle_operator(str, ctx))
		return (0);
	ret = flush_word_buf_as_token(tokens, ctx);
	if (ret != 0)
		return (ret);
	return (dispatch_operator_token(str, tokens, ctx));
}

int	process_operator_token(t_token_type type, const char *literal,
		t_token_list *tokens, t_lexer_context *context)
{
	t_token	*token;

	token = make_operator_token(type, literal);
	if (!token)
	{
		err_malloc_print("tokenizer: operator token");
		free_buf(&context->buf);
		return (-1);
	}
	append_token(tokens, token);
	context->i += ft_strlen(literal);
	return (1);
}

static t_token	*make_operator_token(t_token_type type, const char *literal)
{
	t_token	*token;
	size_t	len;

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
