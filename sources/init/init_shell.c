/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:52 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 16:00:38 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_env_list_empty(t_env_var_list *list);
static char	*find_env_sep(char *env_line);
static t_var	*create_var_from_env_line(char *env_line, char *sep_ptr);
static int	append_var_or_cleanup(t_env_var_list *list, t_var *var);

int	init_env_var_list(t_env_var_list *list, char **envp)
{
	size_t	i;
	char	*sep_ptr;
	t_var	*var;

	init_env_list_empty(list);
	if (!envp)
		return (1);
	i = 0;
	while (envp[i])
	{
		sep_ptr = find_env_sep(envp[i]);
		if (!sep_ptr)
		{
			i++;
			continue ;
		}
		var = create_var_from_env_line(envp[i], sep_ptr);
		if (!var)
			return (0);
		if (!append_var_or_cleanup(list, var))
			return (0);
		i++;
	}
	return (1);
}

static void	init_env_list_empty(t_env_var_list *list)
{
	list->count = 0;
	list->head = NULL;
	list->tail = NULL;
}

static char	*find_env_sep(char *env_line)
{
	return (ft_strchr(env_line, '='));
}

static t_var	*create_var_from_env_line(char *env_line, char *sep_ptr)
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

static int	append_var_or_cleanup(t_env_var_list *list, t_var *var)
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