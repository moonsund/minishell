/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_env_cmds_utils.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:14 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/05 13:22:23 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

void	sort_envp_alpha(char **envp);
void	str_swap(char **s1, char **s2);
int		key_check(char **key, char *argv);
int		check_data(char ***var_data, char *argv, char **key, char **value);
int		key_value_check_init(char **key, char ***var_data, char **value);

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

int	key_check(char **key, char *argv)
{
	int	i;

	i = 0;
	if (!argv || !argv[0])
		return (0);
	*key = argv;
	if (ft_isdigit((*key)[0]))
	{
		err_print(1, "not a valid identifier");
		return (1);
	}
	while ((*key)[i])
	{
		if (!ft_isalnum((*key)[i]) && (!ft_strchr(*key, '_')))
		{
			err_print(1, "not a valid identifier");
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_data(char ***var_data, char *argv, char **key, char **value)
{
	if (argv[0] == '=')
	{
		err_print(1, "not a valid identifier");
		return (0);
	}
	*var_data = ft_split(argv, '=');
	if (!*var_data)
	{
		err_malloc_print("execute_export");
		return (0);
	}
	if (!(*var_data)[0])
	{
		free_strings_array(*var_data);
		return (0);
	}
	if (!key_value_check_init(key, var_data, value))
		return (0);
	return (1);
}

int	key_value_check_init(char **key, char ***var_data, char **value)
{
	int	i;

	i = 0;
	*key = (*var_data)[0];
	if (!(*var_data)[1])
		*value = NULL;
	if ((*var_data)[0] && (*var_data)[1])
		*value = (*var_data)[1];
	if (!*key || ft_isdigit((*key)[0]))
	{
		free_strings_array(*var_data);
		err_print(1, "not a valid identifier");
		return (0);
	}
	while ((*key)[i])
	{
		if (!ft_isalnum((*key)[i]) && (!ft_strchr(*key, '_')))
		{
			free_strings_array(*var_data);
			err_print(1, "not a valid identifier");
			return (0);
		}
		i++;
	}
	return (1);
}
