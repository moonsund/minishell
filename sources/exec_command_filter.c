#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	check_command_type_and_execute(t_shell *minishell)
{
//	minishell->env_variables = build_environment();

	if((ft_strcmp(minishell->pipeline->cmds->argv[0], "echo") == 0) ||
		(ft_strcmp(minishell->pipeline->cmds->argv[0], "cd") == 0) ||
			(ft_strcmp(minishell->pipeline->cmds->argv[0], "pwd") == 0) ||
				(ft_strcmp(minishell->pipeline->cmds->argv[0], "export") == 0) ||
					(ft_strcmp(minishell->pipeline->cmds->argv[0], "unset") == 0) ||
						(ft_strcmp(minishell->pipeline->cmds->argv[0], "env") == 0) ||
							(ft_strcmp(minishell->pipeline->cmds->argv[0], "exit") == 0))
	{
		// No fork needed - Everything is done within the Shell
		execute_built_in_commands(minishell);
	}
	else
	{
		execute_external_commands(minishell);
	}
}
