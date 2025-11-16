#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	check_command_type_and_execute(t_shell minishell/*, TBD */)
{
	// Sujet : Handle environment variables ($ followed by a sequence of characters) which should expand to their values.
	// Verif si presence de variable d'envi - If so, remplacer l'output par la valeur de la variable demandee
	// ex cd $OLDPWD doit faire idem que cd -
	// Voir avec Leo s'il a pris en compte les $var dans le parsing - Decider qui le fait selon temps restant
	if((str_comp(minishell.all_commands.full_command[0], "echo") == 0) ||
		(str_comp(minishell.all_commands.full_command[0], "cd") == 0) ||
			(str_comp(minishell.all_commands.full_command[0], "pwd") == 0) ||
				(str_comp(minishell.all_commands.full_command[0], "export") == 0) ||
					(str_comp(minishell.all_commands.full_command[0], "unset") == 0) ||
						(str_comp(minishell.all_commands.full_command[0], "env") == 0) ||
							(str_comp(minishell.all_commands.full_command[0], "exit") == 0))
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
	minishell.tokens.count = 6;											// I'll get that from Leo's work
	minishell.all_commands.full_command = malloc(sizeof(char*) * minishell.tokens.count);	// Ⓜ️

	minishell.tokens.head = malloc(sizeof(t_token_list));
	minishell.tokens.head->raw_str = ft_strdup("echo");
	minishell.tokens.head->next = malloc(sizeof(t_token_list));
	minishell.tokens.head->next->raw_str = ft_strdup("-n");
	minishell.tokens.head->next->next = malloc(sizeof(t_token_list));
	minishell.tokens.head->next->next->raw_str = ft_strdup("lol");

	int				i = 0;
	t_token_list	*head_copy;
	head_copy = minishell.tokens.head;

	while (i < minishell.tokens.count)
	{
		if(head_copy != NULL)
		{
			minishell.all_commands.full_command[i] = head_copy->head->raw_str;
		}
		i++;
		head_copy = head_copy->head->next;
	}

	// char *raw_str;				// '0\'-terminated string
	// t_token_type type;			// WORD, PIPE, REDIR_IN, etc.
	// t_qmark *quotes_map;		// array of qmarks for everysingle character in the raw_str
	// size_t length;				// *raw_str length
	// struct s_token  *next;


	// minishell.all_commands.next = /* TBC - Built in commands = NULL toujours ? */;

	minishell.env_variables = build_environment();

	check_command_type_and_execute(minishell);

	free_everything(minishell);

	return(0);
}

void	free_everything(t_shell minishell)
{
	free_all_vars(minishell.env_variables);
	free(minishell.all_commands.full_command);
	free(minishell.env_variables);
}
