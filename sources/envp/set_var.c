/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_var.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:16:04 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 16:19:21 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int				set_var(t_env_var_list *list, const char *name,
					const char *value);
static void		append_var_to_list(t_env_var_list *list, t_var *var);
static t_var	*create_new_var(const char *name, const char *value);
static int		update_existing_var(t_var *variable, const char *value);

int	set_var(t_env_var_list *list, const char *name, const char *value)
{
	t_var	*variable;
	t_var	*new_variable;

	if (!list || !name)
		return (0);
	variable = find_var(list, name);
	if (variable)
		return (update_existing_var(variable, value));
	new_variable = create_new_var(name, value);
	if (!new_variable)
		return (0);
	append_var_to_list(list, new_variable);
	return (1);
}

static void	append_var_to_list(t_env_var_list *list, t_var *var)
{
	if (!list->head)
	{
		list->head = var;
		list->tail = var;
	}
	else
	{
		list->tail->next = var;
		list->tail = var;
	}
	list->count++;
}

static t_var	*create_new_var(const char *name, const char *value)
{
	t_var	*var;

	var = malloc(sizeof(*var));
	if (!var)
		return (NULL);
	var->name = ft_strdup(name);
	if (!var->name)
	{
		free(var);
		return (NULL);
	}
	if (value)
		var->value = ft_strdup(value);
	else
		var->value = ft_strdup("");
	if (!var->value)
	{
		free(var->name);
		free(var);
		return (NULL);
	}
	var->next = NULL;
	return (var);
}

static int	update_existing_var(t_var *variable, const char *value)
{
	char	*new_value;

	if (!value)
		return (1);
	new_value = ft_strdup(value);
	if (!new_value)
		return (0);
	free(variable->value);
	variable->value = new_value;
	return (1);
}
