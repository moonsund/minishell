/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:26 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 13:11:53 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int				append_cmd(t_pipeline *pl, t_command cmd);

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
	{
		free_cmd(&ctx.current_cmd);
		free_pipeline(pl);
		return (exit_status);
	}
	if (pl->count == 0)
	{
		free_cmd(&ctx.current_cmd);
		free_pipeline(pl);
		shell->exit_status = 2;
		return (0);
	}
	shell->pipeline = pl;
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
