/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:15 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 13:09:49 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_pipeline	*init_pipeline(void);
void		init_parser_context(t_parser_context *ctx);
void		init_command(t_command *cmd);

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
