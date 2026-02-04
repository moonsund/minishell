/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:46 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 18:20:19 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status			tokenize_with_qmap(const char *str,
							t_token_list *tokens);
int						process_quotes(const char *str, t_lexer_context *ctx);
int						process_spaces_outside_quotes(const char *str,
							t_token_list *tokens, t_lexer_context *ctx);
static t_exit_status	lex_process_step(const char *str, t_token_list *tokens,
							t_lexer_context *ctx);
static t_exit_status	lex_finalize(t_token_list *tokens,
							t_lexer_context *ctx);

t_exit_status	tokenize_with_qmap(const char *str, t_token_list *tokens)
{
	t_lexer_context	ctx;
	t_exit_status	status;

	init_lexer_context(&ctx);
	while (str[ctx.i])
	{
		status = lex_process_step(str, tokens, &ctx);
		if (status != ES_SUCCESS)
			return (status);
	}
	return (lex_finalize(tokens, &ctx));
}

static t_exit_status	lex_process_step(const char *str, t_token_list *tokens,
		t_lexer_context *ctx)
{
	int	res;

	if (process_quotes(str, ctx))
		return (ES_SUCCESS);
	res = process_spaces_outside_quotes(str, tokens, ctx);
	if (res < 0)
		return (free_buf(&ctx->buf), ES_GENERAL);
	if (res > 0)
		return (ES_SUCCESS);
	res = check_operators(str, tokens, ctx);
	if (res < 0)
		return (free_buf(&ctx->buf), ES_GENERAL);
	if (res > 0)
		return (ES_SUCCESS);
	if (!append_char(str[ctx->i], &ctx->buf, ctx->quote_mark))
	{
		free_buf(&ctx->buf);
		return (ES_GENERAL);
	}
	ctx->i++;
	return (ES_SUCCESS);
}

static t_exit_status	lex_finalize(t_token_list *tokens, t_lexer_context *ctx)
{
	if (ctx->in_sq || ctx->in_dq)
	{
		free_buf(&ctx->buf);
		return (ES_INVALID_USAGE);
	}
	if (ctx->buf.used_length > 0)
	{
		if (!process_word(tokens, ctx))
		{
			free_buf(&ctx->buf);
			return (ES_GENERAL);
		}
	}
	free_buf(&ctx->buf);
	return (ES_SUCCESS);
}

int	process_quotes(const char *str, t_lexer_context *ctx)
{
	if (!str || !ctx)
		return (0);
	if (!ctx->in_dq && str[ctx->i] == '\'')
	{
		ctx->in_sq = !ctx->in_sq;
		if (ctx->in_sq)
			ctx->quote_mark = Q_SQ;
		else
			ctx->quote_mark = Q_NONE;
		ctx->i++;
		return (1);
	}
	if (!ctx->in_sq && str[ctx->i] == '\"')
	{
		ctx->in_dq = !ctx->in_dq;
		if (ctx->in_dq)
			ctx->quote_mark = Q_DQ;
		else
			ctx->quote_mark = Q_NONE;
		ctx->i++;
		return (1);
	}
	return (0);
}

int	process_spaces_outside_quotes(const char *str, t_token_list *tokens,
		t_lexer_context *ctx)
{
	if (ctx->in_sq || ctx->in_dq)
		return (0);
	if (!is_space((unsigned char)str[ctx->i]))
		return (0);
	if (ctx->buf.used_length > 0)
	{
		if (!process_word(tokens, ctx))
			return (-1);
		return (1);
	}
	while (is_space((unsigned char)str[ctx->i]))
		ctx->i++;
	return (1);
}
