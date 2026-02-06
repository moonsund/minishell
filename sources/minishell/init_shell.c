/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:00 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/06 13:44:12 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			init_shell(t_shell *shell, char **envp);
static int	init_env_var_list(t_env_var_list *list, char **envp);

int	init_shell(t_shell *shell, char **envp)
{
	shell->exit_status = ES_SUCCESS;
	shell->should_exit = false;
	shell->pl = NULL;
	shell->tokens.count = 0;
	shell->tokens.head = NULL;
	shell->tokens.tail = NULL;
	if (!init_env_var_list(&shell->env_vars, envp))
		return (0);
	return (1);
}

static int	init_env_var_list(t_env_var_list *list, char **envp)
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
