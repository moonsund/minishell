/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_commands.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:42 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/03 17:52:14 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"

char	*build_path_to_check(char *dir, char *cmd);
char	*fetch_and_check_bin_path(t_shell *minishell, char *cmd);
int		execute_external_commands(t_shell *minishell, t_command *cmd);

char	*build_path_to_check(char *dir, char *cmd)
{
	char	*path_to_check;
	char	*tmp;

	path_to_check = ft_strjoin(dir, "/");
	tmp = ft_strdup(path_to_check);
	free(path_to_check);
	path_to_check = ft_strjoin(tmp, cmd);
	free(tmp);
	return (path_to_check);
}

char	*fetch_and_check_bin_path(t_shell *minishell, char *cmd)
{
	t_var	*path_var_in_env;
	char	**path_var_dirs;
	char	*path_to_check;
	int		i;

	i = 0;
	path_var_in_env = find_var(&minishell->env_vars, "PATH");
	if (!path_var_in_env || !path_var_in_env->value)
		return (NULL);
	path_var_dirs = ft_split(path_var_in_env->value, ':');
	if (!path_var_dirs)
		return (NULL);
	while (path_var_dirs[i])
	{
		path_to_check = build_path_to_check(path_var_dirs[i], cmd);
		if (path_to_check && access(path_to_check, X_OK) == 0)
		{
			free_strings_array(path_var_dirs);
			return (path_to_check);
		}
		// if (path_to_check)			// Not necessary
			free(path_to_check);
		i++;
	}
	// Bin command not found = error message dealt with later - Nothing to do here (TBC)
	free_strings_array(path_var_dirs);
	return (NULL);
}

int		execute_external_commands(t_shell *minishell, t_command *cmd)
{
	char	**converted_envp;
	char	**execve_args;
	char	*updated_path;
	bool	path_allocated;

	converted_envp = build_envp(&minishell->env_vars);
	if (!converted_envp)
		return (1);
	path_allocated = false;
	if (cmd->argv)
	{
		execve_args = cmd->argv;
		if (ft_strchr(cmd->argv[0], '/'))				// Command is already entered as binary
		{
			if (access(cmd->argv[0], X_OK) == 0)		// Success = Command is executable
			{
				updated_path = cmd->argv[0];
				path_allocated = false;
			}
			else
			{
				perror("error");
				free_strings_array(converted_envp);
				return (1);
			}
		}
		else
		{
			updated_path = fetch_and_check_bin_path(minishell, execve_args[0]);
			if (!updated_path)
			{
				perror("command not found");
				free_strings_array(converted_envp);
				return (127);
			}
			path_allocated = true;
		}
	}
	else
	{
		free_strings_array(converted_envp);
		return (1);
	}
	execve(updated_path, execve_args, converted_envp);
	// if execve fails :
	perror("command not found");
	if (path_allocated)
		free(updated_path);
	free_strings_array(converted_envp);
	return (126);
}
