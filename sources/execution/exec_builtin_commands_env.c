/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_commands_env.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:14 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/02 21:14:18 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

// The 3 next functions are wrappers for Leo's functions
int		execute_export(t_shell *minishell);
int		execute_unset(t_shell *minishell);
int		execute_env(t_shell *minishell);

// Subject : "export with no options"
int	execute_export(t_shell *minishell)
{
	char	**env_var_data;
	char	**envp_sorted;
	int		i;
	char	*key;
	char	*value;

	if (!minishell->pipeline->cmds->argv[1])
	{
		print_export(minishell);
		return (0);
	}
	env_var_data = ft_split(minishell->pipeline->cmds->argv[1], '=');
	if (!env_var_data)
	{
		err_malloc_print("execute_export");
		return (1);
	}
	key = env_var_data[0];
	value = env_var_data[1];
	i = 0;
	if (!key || ft_isdigit(key[0]))
	{
		free_strings_array(env_var_data);
		err_print(1, "not a valid identifier");
		return (1);
	}
	while (key[i])
	{
		if (!ft_isalnum(key[i]))
		{
			if (!ft_strchr(key, '_'))
			{
				free_strings_array(env_var_data);
				err_print(1, "not a valid identifier");
				return (1);
			}
		}
		i++;
	}
	if (!set_var(&minishell->env_vars, key, value))
	{
		free_strings_array(env_var_data);
		err_print(1, "failed to create environment variable");
		return (1);
	}
	free_strings_array(env_var_data);
	return (0);
}

// Subject : "unset with no options"
int	execute_unset(t_shell *minishell)
{
	unset_var(&minishell->env_vars, minishell->pipeline->cmds->argv[1]);
	return (0);
}

// Subject : "env with no options or arguments"
int	execute_env(t_shell *minishell)
{
	char	**env_to_print;
	int		i;

	env_to_print = build_envp(&minishell->env_vars);
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
