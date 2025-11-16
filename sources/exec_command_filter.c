#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	check_command_type_and_execute(t_shell minishell)
{
	// Sujet : Handle environment variables ($ followed by a sequence of characters) which should expand to their values.
	// Verif si presence de variable d'envi - If so, remplacer l'output par la valeur de la variable demandee
	// ex cd $OLDPWD doit faire idem que cd -
	// Voir avec Leo s'il a pris en compte les $var dans le parsing - Decider qui le fait selon temps restant

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
		// !! Being here means that there is possibly more than one t_command node (check 'next' variable to find out)
		execute_external_commands(minishell);
	}
}

// // Only for testing this file (use current file config in debugger)
// int		main(int ac, char **av)
// {
// 	t_shell	minishell;
// 	minishell.tokens.head = malloc(sizeof(char*) * 4);		// Ⓜ️ Place pour 4 strings pour le 1er node (command + args/flags)

// 	int i = 0;
// 	while (i < 4)
// 	{
// 		if(av[i] != NULL)
// 			minishell.tokens.head[i] = av[i+1];
// 		i++;
// 	}

// 	// minishell.all_commands.next = /* TBC - Built in commands = NULL toujours ? */;

// 	minishell.env_variables = build_environment();

// 	check_command_type_and_execute(minishell);

// 	free_everything(minishell);

// 	return(0);
// }

void	free_everything(t_shell minishell)
{
	free_all_vars(minishell.env_variables);
	// free(minishell.tokens.head);
	free(minishell.env_variables);
}
