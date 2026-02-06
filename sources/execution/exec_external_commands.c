/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_commands.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:42 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/06 13:03:16 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

static int	resolve_path_or_report(t_shell *shell, t_command *cmd,
				char **resolved_path, bool *path_alloc);
static int	exec_with_envp_or_cleanup(t_shell *shell, t_command *cmd,
				char *resolved_path, bool path_alloc);
static int	status_from_errno_for_exec(void);

int	execute_external_commands(t_shell *shell, t_command *cmd)
{
	char	*resolved_path;
	bool	path_alloc;
	int		status;

	if (!shell || !cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	resolved_path = NULL;
	path_alloc = false;
	status = resolve_path_or_report(shell, cmd, &resolved_path, &path_alloc);
	if (status == -1)
		return (0);
	if (status != 0)
		return (status);
	return (exec_with_envp_or_cleanup(shell, cmd, resolved_path, path_alloc));
}

static int	resolve_path_or_report(t_shell *shell, t_command *cmd,
				char **resolved_path, bool *path_alloc)
{
	if (cmd->argv[0][0] == '\0')
		return (-1);
	if (ft_strchr(cmd->argv[0], '/'))
	{
		if (!is_input_exec_ok(cmd->argv[0], resolved_path, path_alloc))
		{
			perror(cmd->argv[0]);
			return (status_from_errno_for_exec());
		}
		return (0);
	}
	if (!is_binary_found(resolved_path, shell, cmd->argv[0], path_alloc))
	{
		print_cmd_not_found(cmd->argv[0]);
		return (127);
	}
	return (0);
}

static int	exec_with_envp_or_cleanup(t_shell *shell, t_command *cmd,
				char *resolved_path, bool path_alloc)
{
	char	**conv_envp;

	conv_envp = build_envp(&shell->env_vars);
	if (!conv_envp)
	{
		if (path_alloc)
			free(resolved_path);
		return (1);
	}
	execve(resolved_path, cmd->argv, conv_envp);
	return (execve_fail(path_alloc, &resolved_path, &conv_envp, resolved_path));
}

static int	status_from_errno_for_exec(void)
{
	if (errno == ENOENT)
		return (127);
	return (126);
}
