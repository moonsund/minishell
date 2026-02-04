/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_cmds_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:42 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/04 14:37:12 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

bool	is_input_exec_ok(char *cmd, char **updt_path, bool *path_alloc);
bool	is_binary_found(char **updt_path, t_shell *shell,
			char *cmd, bool *path_alloc);
char	*fetch_and_check_bin_path(t_shell *shell, char *cmd);
char	*build_path_to_check(char *dir, char *cmd);
void	execve_fail(bool path_alloc, char **updt_path, char ***conv_envp);

bool	is_input_exec_ok(char *cmd, char **updt_path, bool *path_alloc)
{
	if (access(cmd, X_OK) == 0)
	{
		*updt_path = cmd;
		*path_alloc = false;
		return (true);
	}
	else
	{
		perror("error");
		return (false);
	}
}

bool	is_binary_found(char **updt_path, t_shell *shell,
			char *cmd, bool *path_alloc)
{
	*updt_path = fetch_and_check_bin_path(shell, cmd);
	if (!updt_path)
	{
		perror("command not found");
		return (false);
	}
	*path_alloc = true;
	return (true);
}

char	*fetch_and_check_bin_path(t_shell *shell, char *cmd)
{
	t_var	*path_var_in_env;
	char	**path_var_dirs;
	char	*path_to_check;
	int		i;

	i = 0;
	path_var_in_env = find_var(&shell->env_vars, "PATH");
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
		free(path_to_check);
		i++;
	}
	free_strings_array(path_var_dirs);
	return (NULL);
}

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

void	execve_fail(bool path_alloc, char **updt_path, char ***conv_envp)
{
	perror("command not found");
	if (path_alloc)
		free(*updt_path);
	free_strings_array(*conv_envp);
}
