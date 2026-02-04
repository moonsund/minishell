/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_var.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:16:04 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 16:19:26 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int				unset_var(t_env_var_list *list, const char *name);
static void		detach_var_from_list(t_env_var_list *list, t_var *prev,
					t_var *cur);
static void		free_var(t_var *var);
static t_var	*find_var_with_prev(t_env_var_list *list, const char *name,
					t_var **out_prev);

int	unset_var(t_env_var_list *list, const char *name)
{
	t_var	*prev;
	t_var	*cur;

	if (!list || !name)
		return (0);
	cur = find_var_with_prev(list, name, &prev);
	if (!cur)
		return (1);
	detach_var_from_list(list, prev, cur);
	free_var(cur);
	list->count--;
	if (list->count == 0)
		list->tail = NULL;
	return (1);
}

static void	detach_var_from_list(t_env_var_list *list, t_var *prev, t_var *cur)
{
	if (prev == NULL)
		list->head = cur->next;
	else
		prev->next = cur->next;
	if (cur == list->tail)
		list->tail = prev;
}

static void	free_var(t_var *var)
{
	free(var->name);
	free(var->value);
	free(var);
}

static t_var	*find_var_with_prev(t_env_var_list *list, const char *name,
		t_var **out_prev)
{
	t_var	*prev;
	t_var	*cur;

	prev = NULL;
	cur = list->head;
	while (cur)
	{
		if (ft_strcmp(cur->name, name) == 0)
			break ;
		prev = cur;
		cur = cur->next;
	}
	*out_prev = prev;
	return (cur);
}
