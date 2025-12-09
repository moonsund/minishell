#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	check_command_type_and_execute(t_shell *minishell)
{
//	minishell->env_variables = build_environment();

	if(!minishell->tokens.head)
	{
		printf("Plz type something\n");				// Otherwise segfault - See with Leo, should display prompt invite
		return;
	}

	if((ft_strcmp(minishell->tokens.head->raw_str, "echo") == 0) ||
		(ft_strcmp(minishell->tokens.head->raw_str, "cd") == 0) ||
			(ft_strcmp(minishell->tokens.head->raw_str, "pwd") == 0) ||
				(ft_strcmp(minishell->tokens.head->raw_str, "export") == 0) ||
					(ft_strcmp(minishell->tokens.head->raw_str, "unset") == 0) ||
						(ft_strcmp(minishell->tokens.head->raw_str, "env") == 0) ||
							(ft_strcmp(minishell->tokens.head->raw_str, "exit") == 0))
	{
		// No fork needed - Everything is done within the Shell
		execute_built_in_commands(minishell);
	}
	else
	{
		execute_external_commands(minishell);
	}
}
