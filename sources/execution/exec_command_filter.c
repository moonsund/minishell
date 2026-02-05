/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command_filter.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:53 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/05 13:53:13 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int	is_builtin(const char *cmd);
int	is_parent_builtin(const char *cmd);
int	exec_builtin_in_parent(t_shell *shell, t_command *cmd);
int	run_any_builtin_in_child(t_shell *shell, t_command *cmd);
int	execute_built_in_commands(t_shell *shell, t_command *cmd);

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	return (
		ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0
	);
}

int	is_parent_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	return (
		ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "exit") == 0
	);
}

// FD opened and closed right after because these commands don't print anything
int	exec_builtin_in_parent(t_shell *shell, t_command *cmd)
{
	int	new_fd;

	new_fd = -1;
	if (cmd->redirs)
	{
		if (cmd->redirs->type == R_IN)
			new_fd = open_fd(cmd->redirs->target, true, false);
		else if (cmd->redirs->type == R_OUT)
			new_fd = open_fd(cmd->redirs->target, false, true);
		else if (cmd->redirs->type == R_APPEND)
			new_fd = open_fd(cmd->redirs->target, true, false);
		close(new_fd);
	}
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (execute_cd(cmd));
	else if (ft_strcmp(cmd->argv[0], "export") == 0)
		return (execute_export(shell));
	else if (ft_strcmp(cmd->argv[0], "unset") == 0)
		return (execute_unset(shell));
	else if (ft_strcmp(cmd->argv[0], "exit") == 0)
	{
		printf("exit\n");
		return (execute_exit(shell, cmd));
	}
	return (1);
}

// exit should always execute, even in pipeline
int	run_any_builtin_in_child(t_shell *shell, t_command *cmd)
{
	if (is_parent_builtin(cmd->argv[0]))
	{
		if (ft_strcmp(cmd->argv[0], "exit") == 0)
			return (execute_built_in_commands(shell, cmd));
		if (shell->pl->count > 1)
			return (0);
		else
			return (execute_built_in_commands(shell, cmd));
	}
	else
	{
		return (execute_built_in_commands(shell, cmd));
	}
}

// We're in a child process.
// If there are pipes, commands without output have been ignored,
// the others will execute normally with the correct FDs (if applicable)
int	execute_built_in_commands(t_shell *shell, t_command *cmd)
{
	int		exit_status;

	exit_status = 0;
	if (ft_strcmp(cmd->argv[0], "echo") == 0)
		exit_status = execute_echo(cmd);
	else if (ft_strcmp(cmd->argv[0], "cd") == 0)
		exit_status = execute_cd(cmd);
	else if (ft_strcmp(cmd->argv[0], "pwd") == 0)
		exit_status = execute_pwd();
	else if (ft_strcmp(cmd->argv[0], "export") == 0)
		exit_status = execute_export(shell);
	else if (ft_strcmp(cmd->argv[0], "unset") == 0)
		exit_status = execute_unset(shell);
	else if (ft_strcmp(cmd->argv[0], "env") == 0)
		exit_status = execute_env(shell);
	else if (ft_strcmp(cmd->argv[0], "exit") == 0)
	{
		printf("exit\n");
		exit_status = execute_exit(shell, cmd);
		exit(exit_status);
	}
	return (exit_status);
}
