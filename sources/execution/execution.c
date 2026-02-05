/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:31 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 10:47:56 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_pipeline(t_shell *shell)
{
	t_pipeline	*pl;
	t_command	*cmd;

	pl = shell->pipeline;
	if (!pl || pl->count == 0 || !pl->cmds)
		return (0);
	cmd = &pl->cmds[0];
	if (pl->count == 1 && cmd->argv && cmd->argv[0]
		&& is_parent_builtin(cmd->argv[0]))
		return (exec_builtin_in_parent(shell, cmd));
	if ((shell->pipeline->count == 1) && (!shell->pipeline->cmds->argv)
		&& shell->pipeline->cmds->redirs
		&& (shell->pipeline->cmds->redirs->type == R_OUT
			|| shell->pipeline->cmds->redirs->type == R_APPEND))
	{
		open_and_close_fd(cmd);
		return (0);
	}
	return (exec_pipeline_forking(shell, pl));
}