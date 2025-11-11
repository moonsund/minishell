#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	check_command_type_and_execute(t_shell minishell/*, TBD */)
{
	// Sujet : Handle environment variables ($ followed by a sequence of characters) which should expand to their values.
	// Verif si presence de variable d'envi - If so, remplacer l'output par la valeur de la variable demandee
	// ex cd $OLDPWD doit faire idem que cd -
	// Voir avec Leo s'il a pris en compte les $var dans le parsing - Decider qui le fait selon temps restant
	if((ft_memcmp(minishell.all_commands.full_command[0], "echo", 5) == 0) ||
		(ft_memcmp(minishell.all_commands.full_command[0], "cd", 3) == 0) ||
			(ft_memcmp(minishell.all_commands.full_command[0], "pwd", 4) == 0) ||
				(ft_memcmp(minishell.all_commands.full_command[0], "export", 7) == 0) ||
					(ft_memcmp(minishell.all_commands.full_command[0], "unset", 6) == 0) ||
						(ft_memcmp(minishell.all_commands.full_command[0], "env", 4) == 0) ||
							(ft_memcmp(minishell.all_commands.full_command[0], "exit", 5) == 0))
	{
		// TBC - Being here = One node = One command (+ args)
		// No fork needed - Everything is done within the Shell
		execute_built_in_commands(minishell/*, TBD */);
	}
	else
	{
		// !! Being here means that there is possibly more than one t_command node (check 'next' variable to find out)
		execute_external_commands(minishell/*, TBD */);
	}
}

// Only for testing this file (use current file config in debugger)
int		main(int ac, char **av)
{
	t_shell	minishell;
	minishell.all_commands.full_command = malloc(sizeof(char*) * 4);		// Place pour 4 strings pour le 1er node (command + args/flags)

	minishell.all_commands.full_command[0] = ft_strdup(av[1]);			// Ⓜ️
	minishell.all_commands.full_command[1] = ft_strdup(av[2]);			// Ⓜ️
	minishell.all_commands.full_command[2] = ft_strdup(av[3]);			// Ⓜ️
	minishell.all_commands.full_command[3] = ft_strdup(av[4]);			// Ⓜ️
	// minishell.all_commands.next = /* TBC - Built in commands = NULL toujours ? */;

	minishell.env_variables = build_environment();

	check_command_type_and_execute(minishell);

	return(0);
}

