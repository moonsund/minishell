#include "minishell.h"
#include "libft.h"

void	check_command_type_and_execute(t_shell *minishell);
void	execute_built_in_commands(t_shell *minishell);

void	check_command_type_and_execute(t_shell *minishell)
{
//	Is char ** env needed anywhere here ? If so, create it.

	// to put in execute_external_commands when Leo's done with adding redirections to pipeline
	if (minishell->tokens.head->raw_str[0] == '>' || minishell->tokens.head->raw_str[0] == '<')
	{
		execute_external_commands(minishell);
		return;
	}
	// -----------------------------------------------------------------------------------------

	char	*current_command = minishell->pipeline->cmds->argv[0];

	if ((ft_strcmp(current_command, "echo") == 0) ||
		(ft_strcmp(current_command, "cd") == 0) ||
			(ft_strcmp(current_command, "pwd") == 0) ||
				(ft_strcmp(current_command, "export") == 0) ||
					(ft_strcmp(current_command, "unset") == 0) ||
						(ft_strcmp(current_command, "env") == 0))
	{
		// No fork needed - Everything is done within the Shell
		execute_built_in_commands(minishell);
	}
	else
	{
		execute_external_commands(minishell);
	}
}

void	execute_built_in_commands(t_shell *minishell)
{
	char	*current_working_directory;
	current_working_directory = fetch_current_working_directory();												// Ⓜ️

	if(ft_strcmp(minishell->pipeline->cmds->argv[0], "echo") == 0)
		execute_echo(minishell->pipeline->cmds);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "cd") == 0)
		execute_cd(minishell->pipeline->cmds);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "pwd") == 0)
		execute_pwd(current_working_directory);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "export") == 0)
		execute_export(minishell);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "unset") == 0)
		execute_unset(minishell);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "env") == 0)
		execute_env(minishell);
	free(current_working_directory);
}
