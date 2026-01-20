#include "/home/schappuy/00_Root/08_Minishell/includes/minishell.h"
#include "libft.h"

void	check_command_type_and_execute(t_shell *minishell);

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
