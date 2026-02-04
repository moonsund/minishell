/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:16:04 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 16:21:42 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_var	*find_var(t_env_var_list *list, const char *name);
int		set_var(t_env_var_list *list, const char *name, const char *value);
int		unset_var(t_env_var_list *list, const char *name);
char	*get_var_value(t_env_var_list *var_list, const char *var);
char	**build_envp(t_env_var_list *list);

t_var	*find_var(t_env_var_list *list, const char *name)
{
	t_var	*cur;

	if (!list || !name)
		return (NULL);
	cur = list->head;
	while (cur != NULL)
	{
		if (ft_strcmp(cur->name, name) == 0)
			return (cur);
		cur = cur->next;
	}
	return (NULL);
}

char	*get_var_value(t_env_var_list *var_list, const char *var)
{
	t_var	*cur;

	if (!var_list || !var)
		return (NULL);
	cur = var_list->head;
	while (cur != NULL)
	{
		if (ft_strcmp(cur->name, var) == 0)
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}
