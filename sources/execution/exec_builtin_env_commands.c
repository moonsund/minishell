/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_env_commands.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:14 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/05 13:29:11 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int		execute_export(t_shell *shell);
void	print_export(t_shell *shell);
int		execute_unset(t_shell *shell);
int		execute_env(t_shell *shell);

int	execute_export(t_shell *shell)
{
	char	**var_data;
	char	*key;
	char	*val;

	var_data = NULL;
	if (!shell->pl->cmds->argv[1] || shell->pl->cmds->argv[1][0] == '\0')
	{
		print_export(shell);
		return (0);
	}
	if (!ft_strchr(shell->pl->cmds->argv[1], '='))
		return (key_check(&key, shell->pl->cmds->argv[1]));
	else
	{
		if (!check_data(&var_data, shell->pl->cmds->argv[1], &key, &val))
			return (1);
	}
	if (!set_var(&shell->env_vars, key, val))
	{
		free_strings_array(var_data);
		err_print(1, "failed to create environment variable");
		return (1);
	}
	free_strings_array(var_data);
	return (0);
}

void	print_export(t_shell *shell)
{
	char	**envp_to_sort;
	int		i;

	envp_to_sort = build_envp(&shell->env_vars);
	if (!envp_to_sort)
		return ;
	sort_envp_alpha(envp_to_sort);
	i = 0;
	while (envp_to_sort[i])
	{
		printf("declare -x ");
		printf("%s\n", envp_to_sort[i]);
		i++;
	}
	free_strings_array(envp_to_sort);
}

int	execute_unset(t_shell *shell)
{
	unset_var(&shell->env_vars, shell->pl->cmds->argv[1]);
	return (0);
}

int	execute_env(t_shell *shell)
{
	char	**env_to_print;
	int		i;

	env_to_print = build_envp(&shell->env_vars);
	if (!env_to_print)
		return (1);
	i = 0;
	while (env_to_print[i])
	{
		printf("%s\n", env_to_print[i]);
		i++;
	}
	free_strings_array(env_to_print);
	return (0);
}
