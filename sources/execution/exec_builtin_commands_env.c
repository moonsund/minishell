/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_commands_env.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:14 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/02 20:19:15 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int		execute_export(t_shell *minishell);
int		execute_unset(t_shell *minishell);
int		execute_env(t_shell *minishell);
void	sort_envp_alpha(char **envp); // export no args
void	str_swap(char **s1, char **s2);

// The 3 next functions are wrappers for Leo's functions

// Subject : "export with no options"
int	execute_export(t_shell *minishell)
{
	char	**env_var_data;
	char	**envp_to_sort;
	char	**envp_sorted;
	int		i;
	char	*key;
	char	*value;

	if (!minishell->pipeline->cmds->argv[1])
	{
		envp_to_sort = build_envp(&minishell->env_vars);
		sort_envp_alpha(envp_to_sort);
		i = 0;
		while (envp_to_sort[i])
		{
			printf("declare -x ");
			printf("%s\n", envp_to_sort[i]);
			i++;
		}
		free_strings_array(envp_to_sort);
		return (0);
	}
	env_var_data = ft_split(minishell->pipeline->cmds->argv[1], '=');
	key = env_var_data[0];
	value = env_var_data[1];
	// si on rencontre des chars qui ne sont ni alnum, ni _
	i = 0;
	if (!key || ft_isdigit(key[0]))
	{
		err_print(1, "not a valid identifier");
		return (1);
	}
	while (key[i])
	{
		if (!ft_isalnum(key[i]))
		{
			if (!ft_strchr(key, '_'))
			{
				err_print(1, "not a valid identifier");
				return (1);
			}
		}
		i++;
	}
	if (!set_var(&minishell->env_vars, key, value))
	{
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
// Loop through all env vars and print them
int	execute_env(t_shell *minishell)
{
	char	**env_to_print;
	int		i;

	env_to_print = build_envp(&minishell->env_vars);
	i = 0;
	while (env_to_print[i])
	{
		printf("%s\n", env_to_print[i]);
		i++;
	}
	free_strings_array(env_to_print);
	return (0);
}

void	sort_envp_alpha(char **envp)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (envp[i])
	{
		j = i + 1;
		while (envp[j])
		{
			if (ft_strcmp(envp[i], envp[j]) > 0)
			{
				str_swap(&(envp[i]), &(envp[j]));
			}
			j++;
		}
		i++;
	}
}

void	str_swap(char **s1, char **s2)
{
	char	*tmp;

	tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}
