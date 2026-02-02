/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_commands_env_utils.c                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:14 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/02 21:14:40 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

void	print_export(t_shell *minishell);
void	sort_envp_alpha(char **envp);
void	str_swap(char **s1, char **s2);

void	print_export(t_shell *minishell)
{
	char	**envp_to_sort;
	int		i;

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
