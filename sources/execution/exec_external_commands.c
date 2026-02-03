/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_commands.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:42 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/03 20:21:29 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int	execute_external_commands(t_shell *minishell, t_command *cmd);

int	execute_external_commands(t_shell *minishell, t_command *cmd)
{
	char	**conv_envp;
	char	*updated_path;
	bool	path_alloc;

	path_alloc = false;
	if (cmd->argv)
	{
		if (ft_strchr(cmd->argv[0], '/'))
		{
			if (!is_input_exec_ok(cmd->argv[0], &updated_path, &path_alloc))
				return (1);
		}
		else
			if (!is_cmd_binary_found(&updated_path, minishell, cmd->argv[0], &path_alloc))
				return (127);
	}
	else
		return (1);
	conv_envp = build_envp(&minishell->env_vars);
	if (!conv_envp)
		return (1);
	execve(updated_path, cmd->argv, conv_envp);
	execve_fail(path_alloc, &updated_path, &conv_envp);
	return (126);
}
