#include "minishell.h"
#include "libft.h"

void	execute_export(t_shell *minishell);
void	execute_unset(t_shell *minishell);
void	execute_env(t_shell *minishell);

// The 3 next functions are wrappers for Leo's functions

// Subject : "export with no options"
// Loop through variables. If name not found, create note and add node to list. If found, edit the value
void	execute_export(t_shell *minishell)
{
	char	**env_var_data;

	if(!minishell->pipeline->cmds->argv[1])
	{
		execute_env(minishell);			// In the meantime (see below)
		// Print env in alpha order
		// char **envp_to_sort = build_envp(&minishell->env_vars);
		// char **envp_sorted = sort_envp_alpha(envp_to_sort);

		// int i = 0;
		// while (envp_sorted[i])
		// {
		// 	printf("%s\n", envp_sorted[i]);
		// 	i++;
		// }
	}
	env_var_data = ft_split(minishell->pipeline->cmds->argv[1], '=');

	char	*key = env_var_data[0];
	char	*value = env_var_data[1];
	if (!set_var(&minishell->env_vars, key, value))
	{
		err_print(1, "failed to create environment variable");
	}
	free_ft_split_output(env_var_data);
}

// Subject : "unset with no options"
void	execute_unset(t_shell *minishell)
{
	if (!unset_var(&minishell->env_vars, minishell->pipeline->cmds->argv[1]))
	{
		err_print(1, "failed to unset environment variable");
	}
}
// Subject : "env with no options or arguments"
// Loop through all env vars and print them
void	execute_env(t_shell *minishell)
{
	char **env_to_print = build_envp(&minishell->env_vars);
	int i = 0;
	while (env_to_print[i])
	{
		printf("%s\n", env_to_print[i]);
		i++;
	}
}
