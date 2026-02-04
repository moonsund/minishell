/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:46 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 10:15:47 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void init_lexer_context(t_lexer_context *context);
int process_quotes(const char *str, t_lexer_context *ctx);
int process_spaces_outside_quotes(const char *str, t_token_list *tokens, t_lexer_context *ctx);

t_exit_status tokenize_with_qmap(const char *str, t_token_list *tokens)
{
	t_lexer_context ctx;
	int sp_res;
	int op_res;

	init_lexer_context(&ctx);

	while (str[ctx.i])
	{
		if (process_quotes(str, &ctx))
			continue;

		// process white spaces outside quotes
		sp_res = process_spaces_outside_quotes(str, tokens, &ctx);
		if (sp_res < 0)
			return (ES_GENERAL);
		if (sp_res > 0)
			continue;

		// process operators outside quotes
		op_res = check_operators(str, tokens, &ctx);
		if (op_res < 0)
			return (ES_GENERAL);
		if (op_res > 0)
			continue;

		// accumulate characters in the buffer
		if (!append_char(str[ctx.i], &ctx.buf, ctx.quote_mark))
		{
			free_buf(&ctx.buf);
			return (ES_GENERAL);
		}
		ctx.i++;
	}

	if (ctx.in_sq || ctx.in_dq)
	{
		free_buf(&ctx.buf);
		return (ES_INVALID_USAGE);
	}

	if (ctx.buf.used_length > 0)
	{
		if (!process_word(tokens, &ctx))
			return (ES_GENERAL);
	}

	free_buf(&ctx.buf);
	return (ES_SUCCESS);
}

void init_lexer_context(t_lexer_context *context)
{
	init_buffer(&context->buf);

	context->quote_mark = Q_NONE;
	context->i = 0;
	context->in_dq = false;
	context->in_sq = false;
}

int process_quotes(const char *str, t_lexer_context *ctx)
{
	if (!str || !ctx)
		return (0);

	char c;

	c = str[ctx->i];
	if (!ctx->in_dq && c == '\'')
		{
			ctx->in_sq = !ctx->in_sq;
			if (ctx->in_sq)
				ctx->quote_mark = Q_SQ;
			else
				ctx->quote_mark = Q_NONE;
			ctx->i++;
			return (1);
		}
	if (!ctx->in_sq && c == '\"')
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

int process_spaces_outside_quotes(const char *str, t_token_list *tokens, t_lexer_context *ctx)
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
