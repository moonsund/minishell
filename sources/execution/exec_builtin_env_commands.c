/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_env_commands.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:14 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/04 14:18:01 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int	execute_export(t_shell *shell);
int	execute_unset(t_shell *shell);
int	execute_env(t_shell *shell);

// Subject : "export with no options"
int	execute_export(t_shell *shell)
{
	char	*argv_1;
	char	**var_data;
	char	*key;
	char	*value;

	if (!shell->pipeline->cmds->argv[1])
	{
		print_export(shell);
		return (0);
	}
	argv_1 = shell->pipeline->cmds->argv[1];
	if (!check_var_data(&var_data, argv_1, &key, &value))
		return (1);
	if (!set_var(&shell->env_vars, key, value))
	{
		free_strings_array(var_data);
		err_print(1, "failed to create environment variable");
		return (1);
	}
	free_strings_array(var_data);
	return (0);
}

// Subject : "unset with no options"
int	execute_unset(t_shell *shell)
{
	unset_var(&shell->env_vars, shell->pipeline->cmds->argv[1]);
	return (0);
}

// Subject : "env with no options or arguments"
int	execute_env(t_shell *shell)
{
	char	**env_to_print;
	int		i;

	env_to_print = build_envp(&shell->env_vars);
	if (!env_to_print)
		return (1);
	i = 0;
	while (env_to_print[i])
	{
		printf("%s\n", env_to_print[i]);
		i++;
	}
	free_strings_array(env_to_print);
	return (0);
}
