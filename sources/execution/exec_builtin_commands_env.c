#include "minishell.h"
#include "libft.h"

void	execute_export(t_shell *minishell);
void	execute_unset(t_shell *minishell);
void	execute_env(t_shell *minishell);
void	sort_envp_alpha(char **envp);								// export no args
void	str_swap(char **s1, char **s2);

// The 3 next functions are wrappers for Leo's functions

// Subject : "export with no options"
// Loop through variables. If name not found, create note and add node to list. If found, edit the value
void	execute_export(t_shell *minishell)
{
	char	**env_var_data;
	char	**envp_to_sort;
	char	**envp_sorted;

	if(!minishell->pipeline->cmds->argv[1])
	{
		envp_to_sort = build_envp(&minishell->env_vars);
		sort_envp_alpha(envp_to_sort);

		int i = 0;
		while (envp_to_sort[i])
		{
			printf("declare -x ");
			printf("%s\n", envp_to_sort[i]);
			i++;
		}
		free_strings_array(envp_to_sort);
		return;
	}
	env_var_data = ft_split(minishell->pipeline->cmds->argv[1], '=');

	char	*key = env_var_data[0];
	char	*value = env_var_data[1];
	if (!set_var(&minishell->env_vars, key, value))
	{
		err_print(1, "failed to create environment variable");
	}
	free_strings_array(env_var_data);
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
	free_strings_array(env_to_print);
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
