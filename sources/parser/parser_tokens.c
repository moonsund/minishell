/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:20 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 00:10:58 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int				process_word_token(t_parser_context *ctx);
t_exit_status	process_pipe_token(t_pipeline *pl, t_parser_context *ctx);
t_exit_status	process_redir_tokens(t_parser_context *ctx);
int				append_arg(t_command *cmd, char *arg);

int	process_word_token(t_parser_context *ctx)
{
	if (!ctx->cmd_started)
	{
		init_command(&ctx->current_cmd);
		ctx->cmd_started = 1;
	}
	if (!append_arg(&ctx->current_cmd, ctx->current->raw_str))
	{
		err_malloc_print("parser: append arg");
		return (0);
	}
	return (1);
}

t_exit_status	process_pipe_token(t_pipeline *pl, t_parser_context *ctx)
{
	if (!ctx->cmd_started || !ctx->next || ctx->current_cmd.argv == NULL
		|| ctx->current_cmd.argv[0] == NULL)
	{
		err_print(ES_INVALID_USAGE, "parse error near '|'");
		return (ES_INVALID_USAGE);
	}
	if (!append_cmd(pl, ctx->current_cmd))
		return (ES_GENERAL);
	init_command(&ctx->current_cmd);
	ctx->cmd_started = 0;
	return (ES_SUCCESS);
}

t_exit_status	process_redir_tokens(t_parser_context *ctx)
{
	t_redir	*redir_node;

	if (!ctx->cmd_started)
	{
		init_command(&ctx->current_cmd);
		ctx->cmd_started = 1;
	}
	if (!ctx->next || ctx->next->type != TOK_WORD)
	{
		err_print(ES_INVALID_USAGE, "expected filename or limiter");
		return (ES_INVALID_USAGE);
	}
	redir_node = init_redirect(ctx);
	if (!redir_node)
	{
		err_malloc_print("parser: redir node");
		return (ES_GENERAL);
	}
	if (!redir_push_back(&ctx->current_cmd.redirs, redir_node))
		return (free(redir_node->target), free(redir_node), ES_GENERAL);
	ctx->current = ctx->next;
	return (ES_SUCCESS);
}

int	append_arg(t_command *cmd, char *arg)
{
	char	**new_argv;
	size_t	argc;

	if (!cmd)
		return (0);
	if (!arg)
		return (1);
	argc = count_argv(cmd->argv);
	new_argv = alloc_argv_with_copy(cmd->argv, argc);
	if (!new_argv)
		return (0);
	if (!append_dup_arg(new_argv, argc, arg))
	{
		free(new_argv);
		return (0);
	}
	free(cmd->argv);
	cmd->argv = new_argv;
	return (1);
}
