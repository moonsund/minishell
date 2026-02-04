/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_envp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:16:04 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 16:21:30 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char		**build_envp(t_env_var_list *list);
static int	fill_envp_from_list(char **envp, t_env_var_list *list);
static char	*make_envp_entry(const t_var *var);

char	**build_envp(t_env_var_list *list)
{
	char	**envp;

	if (!list)
		return (NULL);
	envp = malloc(sizeof(*envp) * (list->count + 1));
	if (!envp)
		return (NULL);
	if (!fill_envp_from_list(envp, list))
		return (NULL);
	return (envp);
}

static int	fill_envp_from_list(char **envp, t_env_var_list *list)
{
	size_t	i;
	t_var	*cur;

	i = 0;
	cur = list->head;
	while (cur)
	{
		envp[i] = make_envp_entry(cur);
		if (!envp[i])
		{
			free_envp_partial(envp, i);
			return (0);
		}
		i++;
		cur = cur->next;
	}
	envp[i] = NULL;
	return (1);
}

static char	*make_envp_entry(const t_var *var)
{
	char	*tmp;
	char	*entry;

	tmp = ft_strjoin(var->name, "=");
	if (!tmp)
		return (NULL);
	if (var->value)
		entry = ft_strjoin(tmp, var->value);
	else
		entry = ft_strjoin(tmp, "");
	free(tmp);
	return (entry);
}
