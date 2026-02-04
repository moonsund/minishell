/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_operators_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:46 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 18:20:34 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	should_handle_operator(const char *str, const t_lexer_context *ctx);
int	flush_word_buf_as_token(t_token_list *tokens, t_lexer_context *ctx);
int	dispatch_operator_token(const char *str, t_token_list *tokens,
		t_lexer_context *ctx);

int	should_handle_operator(const char *str, const t_lexer_context *ctx)
{
	if (ctx->in_sq || ctx->in_dq)
		return (0);
	return (is_operator(str[ctx->i]));
}

int	flush_word_buf_as_token(t_token_list *tokens, t_lexer_context *ctx)
{
	t_token	*token;

	if (ctx->buf.used_length == 0)
		return (0);
	token = make_word_token(&ctx->buf);
	if (!token)
	{
		err_malloc_print("tokenizer: word token");
		free_buf(&ctx->buf);
		return (-1);
	}
	append_token(tokens, token);
	reset_buf(&ctx->buf);
	return (0);
}

int	dispatch_operator_token(const char *str, t_token_list *tokens,
		t_lexer_context *ctx)
{
	if (str[ctx->i] == '|')
		return (process_operator_token(TOK_PIPE, "|", tokens, ctx));
	if (str[ctx->i] == '<')
	{
		if (str[ctx->i + 1] == '<')
			return (process_operator_token(TOK_HEREDOC, "<<", tokens, ctx));
		return (process_operator_token(TOK_REDIR_IN, "<", tokens, ctx));
	}
	if (str[ctx->i] == '>')
	{
		if (str[ctx->i + 1] == '>')
			return (process_operator_token(TOK_APPEND, ">>", tokens, ctx));
		return (process_operator_token(TOK_REDIR_OUT, ">", tokens, ctx));
	}
	return (0);
}
