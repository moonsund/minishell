#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	execute_built_in_commands(t_shell minishell/*, TBD */) // *minishell
{
	char	*current_working_directory;
	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);		// calloc dont need everytime		// Ⓜ️
	getcwd(current_working_directory, PATH_MAX);

	if(str_comp(minishell.tokens.head->raw_str, "echo") == 0)
	{
		execute_echo(minishell.tokens.head, minishell);
	}
	else if(str_comp(minishell.tokens.head->raw_str, "cd") == 0)
	{
		ft_printf("TestPrint cd command old path :\t%s\n", current_working_directory);
		execute_cd(current_working_directory, minishell.tokens.head);
		// current_working_directory = ft_calloc(sizeof(char), PATH_MAX);					// Comment out for debug
		// getcwd(current_working_directory, PATH_MAX);										// Comment out for debug
		// ft_printf("TestPrint cd command new path :\t%s\n\n", current_working_directory);	// Comment out for debug
	}
	else if(str_comp(minishell.tokens.head->raw_str, "pwd") == 0)
	{
		execute_pwd(current_working_directory);
	}
	else if(str_comp(minishell.tokens.head->raw_str, "export") == 0)
	{
		// execute_env(minishell);															// Comment out for debug
		// write(1, "\n\n", 2);																// Comment out for debug
		execute_export(minishell);
		// execute_env(minishell);															// Comment out for debug
	}
	else if(str_comp(minishell.tokens.head->raw_str, "unset") == 0)
	{
		// execute_env(minishell);															// Comment out for debug
		// write(1, "\n\n", 2);																// Comment out for debug
		execute_unset(minishell);
		// execute_env(minishell);															// Comment out for debug
	}
	else if(str_comp(minishell.tokens.head->raw_str, "env") == 0)
	{
		execute_env(minishell);
	}
	else if(str_comp(minishell.tokens.head->raw_str, "exit") == 0)
	{
		free(current_working_directory);
		execute_exit(minishell);
	}
	free(current_working_directory);
}
// Subject : "echo with option -n"
// Check if string has been placed in a char* , or if each word is a char* -> VERY PROBABLY - Check w/ Leonid
void	execute_echo(t_token *first_command, t_shell minishell)
{
	bool	line_return = true;
	char	*cropped_var;
	char	*fetched_value;
	first_command = first_command->next;
	if(str_comp(first_command->raw_str, "-n") == 0)
	{
		first_command = first_command->next;
		line_return = false;
	}
	while (first_command != NULL)
	{
		if (first_command->raw_str[0] == '$')				// Maybe there is an easier/shorter way, see with Leo if parsing can replace a key by its value
		{
			cropped_var = ft_strchr(first_command->raw_str, '$') + 1;
			fetched_value = fetch_value_from_key(minishell.env_variables, cropped_var);
			ft_printf("%s", fetched_value);
		}
		else
		{
			ft_printf("%s", first_command->raw_str);
		}
		first_command = first_command->next;
		if(first_command)
			write(1, " ", 1);
	}
	if(line_return == true)
		write(1, "\n", 1);
}

// Subject : "cd with only a relative or absolute path"
void	execute_cd(char *current_working_directory, t_token *first_command)
{
	if(chdir(first_command->next->raw_str) == -1)
	{
		perror("Error");											// Errno prints the rest of the message
	}
}

// Subject : "pwd with no options"
void	execute_pwd(char *current_working_directory)
{
	ft_printf("%s\n", current_working_directory);
}

// Subject : "export with no options"
// Loop through variables. If name not found, create note and add node to list. If found, edit the value
void	execute_export(t_shell minishell)
{
	t_env	*new_environment_variable;
	char	**split = ft_split(minishell.tokens.head->next->raw_str, '=');

	char	*key = split[0];														// Ⓜ️
	char	*value = split[1];														// Ⓜ️
	new_environment_variable = create_new_environment_variable(key, value);
	add_env_var_to_list(minishell.env_variables, new_environment_variable);
}

// Subject : "unset with no options"
void	execute_unset(t_shell minishell)
{
	delete_env_var(minishell.env_variables, minishell.tokens.head->next->raw_str);
}
// Subject : "env with no options or arguments"
// Loop through all env vars and print them
void	execute_env(t_shell minishell)
{
	t_env	*backup_ptr;
	backup_ptr = *(minishell.env_variables);
	while (backup_ptr != NULL)
	{
		ft_printf("%s=%s\n", backup_ptr->variable_name, backup_ptr->variable_data);
		backup_ptr = backup_ptr->next;
	}
}

// Subject : "exit with no options"
// Memory leaks since I merged parsing & exec
void	execute_exit(t_shell minishell)
{
	free_everything(minishell);
	exit(0);
}
