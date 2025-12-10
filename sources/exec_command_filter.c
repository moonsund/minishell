#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	check_command_type_and_execute(t_shell *minishell)
{
//	minishell->env_variables = build_environment();
	char	*current_command = minishell->pipeline->cmds->argv[0];

	if((ft_strcmp(current_command, "echo") == 0) ||
		(ft_strcmp(current_command, "cd") == 0) ||
			(ft_strcmp(current_command, "pwd") == 0) ||
				(ft_strcmp(current_command, "export") == 0) ||
					(ft_strcmp(current_command, "unset") == 0) ||
						(ft_strcmp(current_command, "env") == 0) ||
							(ft_strcmp(current_command, "exit") == 0))
	{
		// No fork needed - Everything is done within the Shell
		execute_built_in_commands(minishell);
	}
	else
	{
		execute_external_commands(minishell);
	}
}
