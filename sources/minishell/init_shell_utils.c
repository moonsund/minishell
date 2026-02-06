/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:52 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/06 13:35:33 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_env_list_empty(t_env_var_list *list);
char	*find_env_sep(char *env_line);
t_var	*create_var_from_env_line(char *env_line, char *sep_ptr);
int		append_var_or_cleanup(t_env_var_list *list, t_var *var);

void	init_env_list_empty(t_env_var_list *list)
{
	list->count = 0;
	list->head = NULL;
	list->tail = NULL;
}

char	*find_env_sep(char *env_line)
{
	return (ft_strchr(env_line, '='));
}

t_var	*create_var_from_env_line(char *env_line, char *sep_ptr)
{
	t_var	*var;

	var = malloc(sizeof(*var));
	if (!var)
		return (NULL);
	var->name = ft_substr(env_line, 0, (size_t)(sep_ptr - env_line));
	var->value = ft_strdup(sep_ptr + 1);
	var->next = NULL;
	if (!var->name || !var->value)
	{
		free(var->name);
		free(var->value);
		free(var);
		return (NULL);
	}
	return (var);
}

int	append_var_or_cleanup(t_env_var_list *list, t_var *var)
{
	if (!var)
		return (0);
	if (list->head == NULL)
		list->head = var;
	else
		list->tail->next = var;
	list->tail = var;
	list->count++;
	return (1);
}
