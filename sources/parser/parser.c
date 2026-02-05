/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:26 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 13:29:31 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status			build_pipeline_from_tokens(t_shell *shell);
static t_exit_status	process_tokens(t_pipeline *pl, t_token_list *list,
							t_parser_context *ctx);
static t_exit_status	process_one_token(t_pipeline *pl,
							t_parser_context *ctx);
static t_exit_status	finalize_last_command(t_pipeline *pl,
							t_parser_context *ctx);
int						append_cmd(t_pipeline *pl, t_command cmd);

t_exit_status	build_pipeline_from_tokens(t_shell *shell)
{
	t_parser_context	ctx;
	t_pipeline			*pl;
	t_exit_status		exit_status;

	if (!shell)
		return (ES_GENERAL);
	init_parser_context(&ctx);
	pl = init_pipeline();
	if (!pl)
		return (ES_GENERAL);
	exit_status = process_tokens(pl, &shell->tokens, &ctx);
	if (exit_status != ES_SUCCESS)
		return (free_cmd(&ctx.current_cmd), free_pipeline(pl), exit_status);
	if (pl->count == 0)
	{
		free_cmd(&ctx.current_cmd);
		free_pipeline(pl);
		shell->exit_status = 2;
		return (0);
	}
	shell->pl = pl;
	return (ES_SUCCESS);
}

static t_exit_status	process_tokens(t_pipeline *pl, t_token_list *list,
		t_parser_context *ctx)
{
	t_exit_status	exit_status;

	ctx->current = list->head;
	while (ctx->current)
	{
		ctx->next = ctx->current->next;
		exit_status = process_one_token(pl, ctx);
		if (exit_status != ES_SUCCESS)
			return (exit_status);
		ctx->current = ctx->current->next;
	}
	return (finalize_last_command(pl, ctx));
}

static t_exit_status	process_one_token(t_pipeline *pl, t_parser_context *ctx)
{
	t_exit_status	exit_status;

	if (ctx->current->type == TOK_WORD)
	{
		if (!process_word_token(ctx))
			return (ES_GENERAL);
		return (ES_SUCCESS);
	}
	if (ctx->current->type == TOK_PIPE)
	{
		exit_status = process_pipe_token(pl, ctx);
		if (exit_status != ES_SUCCESS)
			return (exit_status);
		return (ES_SUCCESS);
	}
	exit_status = process_redir_tokens(ctx);
	if (exit_status != ES_SUCCESS)
		return (exit_status);
	return (ES_SUCCESS);
}

static t_exit_status	finalize_last_command(t_pipeline *pl,
		t_parser_context *ctx)
{
	int	has_argv;
	int	has_redirs;

	if (!ctx->cmd_started)
		return (ES_SUCCESS);
	has_argv = (ctx->current_cmd.argv && ctx->current_cmd.argv[0]);
	has_redirs = (ctx->current_cmd.redirs != NULL);
	if (!has_argv && !has_redirs)
	{
		err_print(ES_INVALID_USAGE, "near 'newline'");
		return (ES_INVALID_USAGE);
	}
	if (!append_cmd(pl, ctx->current_cmd))
		return (ES_GENERAL);
	return (ES_SUCCESS);
}

int	append_cmd(t_pipeline *pl, t_command cmd)
{
	t_command	*new_cmds;
	size_t		i;

	new_cmds = malloc(sizeof(*new_cmds) * (pl->count + 1));
	if (!new_cmds)
	{
		err_malloc_print("append command");
		return (0);
	}
	i = 0;
	while (i < pl->count)
	{
		new_cmds[i] = pl->cmds[i];
		i++;
	}
	new_cmds[pl->count] = cmd;
	free(pl->cmds);
	pl->cmds = new_cmds;
	pl->count++;
	return (1);
}
