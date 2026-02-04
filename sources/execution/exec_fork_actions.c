/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_fork_actions.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:31 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 23:45:55 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int	wait_all_and_get_last(pid_t *pids, size_t count);

// int	in_child()
// {
// 		// if (pid == 0)
// 		// {
// 		// 	if (prev_read != -1)
// 		// 	{
// 		// 		if (dup2(prev_read, STDIN_FILENO) < 0)
// 		// 		{
// 		// 			perror("dup2 stdin");
// 		// 			exit(1);
// 		// 		}
// 		// 	}
// 		// 	if (pipefds[1] != -1)
// 		// 	{
// 		// 		if (dup2(pipefds[1], STDOUT_FILENO) < 0)
// 		// 		{
// 		// 			perror("dup2 stdout");
// 		// 			exit(1);
// 		// 		}
// 		// 	}
// 		// 	close_all_if_valid(&prev_read, pipefds, false);
// 		// 	apply_redirs_or_die(&pl->cmds[i]);
// 		// 	if (!pl->cmds[i].argv || !pl->cmds[i].argv[0])
// 		// 		exit(0);
// 		// 	if (is_builtin(pl->cmds[i].argv[0]))
// 		// 	{
// 		// 		last_status = run_any_builtin_in_child(shell, &pl->cmds[i]);
// 		// 		exit(last_status);
// 		// 	}
// 		// 	else
// 		// 		exit (execute_external_commands(shell, &pl->cmds[i]));
// 		// }
// }

int	wait_all_and_get_last(pid_t *pids, size_t count)
{
	size_t	i;
	int		status;
	int		last_status;
	pid_t	last_pid;

	last_status = 0;
	last_pid = pids[count - 1];
	i = 0;
	while (i < count)
	{
		if (waitpid(pids[i], &status, 0) > 0)
		{
			if (pids[i] == last_pid)
			{
				if (WIFEXITED(status))
					last_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					last_status = 128 + WTERMSIG(status);
				else
					last_status = 1;
			}
		}
		i++;
	}
	return (last_status);
}
