#include "minishell.h"
#include "libft.h"

void	execute_built_in_commands(t_shell *minishell);
char	*fetch_current_working_directory(void);
void	execute_echo(t_command *cmds);

void	execute_built_in_commands(t_shell *minishell)
{
	char	*current_working_directory;
	current_working_directory = fetch_current_working_directory();												// Ⓜ️

	if(ft_strcmp(minishell->pipeline->cmds->argv[0], "echo") == 0)
	{
		execute_echo(minishell->pipeline->cmds);
	}
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "cd") == 0)
	{
		printf("DEBUG - Old path :\t%s\n", current_working_directory);
		execute_cd(minishell->pipeline->cmds);
		// current_working_directory = ft_calloc(sizeof(char), PATH_MAX);					// Comment out for debug
		// getcwd(current_working_directory, PATH_MAX);										// Comment out for debug
		// printf("DEBUG - New path :\t%s\n", current_working_directory);					// Comment out for debug
	}
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "pwd") == 0)
	{
		execute_pwd(current_working_directory);
	}
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "export") == 0)
	{
		execute_export(minishell);
	}
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "unset") == 0)
	{
		execute_unset(minishell);
	}
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "env") == 0)
	{
		execute_env(minishell);
	}
	// else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "exit") == 0)
	// {
	// 	free(current_working_directory);
	// 	execute_exit(minishell);
	// }
	free(current_working_directory);
}

char	*fetch_current_working_directory(void)
{
	char	*current_working_directory;
	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);						// Ⓜ️
	if (!current_working_directory)
	{
		perror("");
		return(NULL);
	}
	getcwd(current_working_directory, PATH_MAX);
	return (current_working_directory);
}

// Subject : "echo with option -n"
void	execute_echo(t_command *cmds)
{
	bool	line_return = true;
	int		i = 1;

	if (cmds->argv[1] && (ft_strcmp(cmds->argv[1], "-n")) == 0)
	{
		line_return = false;
		i++;
	}
	while(cmds->argv[i])
	{
		printf("%s", cmds->argv[i]);
		fflush(0);									// Only for debug
		if(cmds->argv[i+1])
		{
			write(1, " ", 1);
		}
		i++;
	}
	if(line_return == true)
		write(1, "\n", 1);
	line_return = true;
}

// Subject : "cd with only a relative or absolute path"
void	execute_cd(t_command *cmds)
{
	if (cmds->argv[1])
	{
		if(chdir(cmds->argv[1]) == -1)
		{
			perror("Error");											// Errno prints the rest of the message
		}
	}
}

// Subject : "pwd with no options"
void	execute_pwd(char *current_working_directory)
{
	printf("%s\n", current_working_directory);
}

// The 3 next functions are wrappers for Leo's functions

// Subject : "export with no options"
// Loop through variables. If name not found, create note and add node to list. If found, edit the value
void	execute_export(t_shell *minishell)
{
	char	**split = ft_split(minishell->pipeline->cmds->argv[1], '=');

	char	*key = split[0];														// Ⓜ️
	char	*value = split[1];														// Ⓜ️
	if (!set_var(&minishell->env_vars, key, value))
	{
		err_print(1, "failed to create environment variable");
	}
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
