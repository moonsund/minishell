/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:15 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 21:43:50 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_pipeline	*init_pipeline(void);
void		init_parser_context(t_parser_context *ctx);
void		init_command(t_command *cmd);
t_redir		*init_redirect(t_parser_context *ctx);
static void	apply_redir(t_redir *redir, t_token_type token_type,
				t_parser_context *ctx);

t_pipeline	*init_pipeline(void)
{
	t_pipeline	*pl;

	pl = malloc(sizeof(*pl));
	if (!pl)
	{
		err_malloc_print("init_pipeline");
		return (NULL);
	}
	pl->cmds = NULL;
	pl->count = 0;
	return (pl);
}

void	init_parser_context(t_parser_context *ctx)
{
	init_command(&ctx->current_cmd);
	ctx->current = NULL;
	ctx->next = NULL;
	ctx->tmp = NULL;
	ctx->cmd_started = 0;
}

void	init_command(t_command *cmd)
{
	cmd->argv = NULL;
	cmd->redirs = NULL;
}

t_redir	*init_redirect(t_parser_context *ctx)
{
	t_redir			*redir;
	t_token_type	token_type;

	if (!ctx || !ctx->current || !ctx->next || !ctx->next->raw_str)
		return (NULL);
	token_type = ctx->current->type;
	redir = (t_redir *)malloc(sizeof(*redir));
	if (!redir)
		return (NULL);
	redir->fd = 0;
	redir->expand = 0;
	redir->next = NULL;
	apply_redir(redir, token_type, ctx);
	redir->target = ft_strdup(ctx->next->raw_str);
	if (!redir->target)
	{
		free(redir);
		return (NULL);
	}
	return (redir);
}

static void	apply_redir(t_redir *redir, t_token_type token_type,
		t_parser_context *ctx)
{
	if (token_type == TOK_REDIR_IN)
	{
		redir->type = R_IN;
		redir->fd = 0;
	}
	else if (token_type == TOK_HEREDOC)
	{
		redir->type = R_HEREDOC;
		redir->fd = 0;
		if (ctx && ctx->next && !token_has_any_quotes(ctx->next))
			redir->expand = 1;
	}
	else if (token_type == TOK_REDIR_OUT)
	{
		redir->type = R_OUT;
		redir->fd = 1;
	}
	else
	{
		redir->type = R_APPEND;
		redir->fd = 1;
	}
}
