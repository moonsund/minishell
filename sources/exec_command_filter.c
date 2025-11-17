#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	check_command_type_and_execute(t_shell minishell)
{
	minishell.env_variables = build_environment();

	if((str_comp(minishell.tokens.head->raw_str, "echo") == 0) ||
		(str_comp(minishell.tokens.head->raw_str, "cd") == 0) ||
			(str_comp(minishell.tokens.head->raw_str, "pwd") == 0) ||
				(str_comp(minishell.tokens.head->raw_str, "export") == 0) ||
					(str_comp(minishell.tokens.head->raw_str, "unset") == 0) ||
						(str_comp(minishell.tokens.head->raw_str, "env") == 0) ||
							(str_comp(minishell.tokens.head->raw_str, "exit") == 0))
	{
		// No fork needed - Everything is done within the Shell
		execute_built_in_commands(minishell);
	}
	else
	{
		// execute_external_commands(minishell);
		return;		// To delete when ext commands done
	}
	free_everything(minishell);
}

void	free_everything(t_shell minishell)
{
	free_all_vars(minishell.env_variables);
	// free(minishell.tokens.head);
	free(minishell.env_variables);
}
