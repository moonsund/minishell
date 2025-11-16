#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	execute_built_in_commands(t_shell minishell/*, TBD */)
{
	char	*current_working_directory;
	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);						// Ⓜ️
	getcwd(current_working_directory, PATH_MAX);

	if(str_comp(minishell.all_commands.full_command[0], "echo") == 0)
	{
		execute_echo(minishell.all_commands.full_command);
	}
	else if(str_comp(minishell.all_commands.full_command[0], "cd") == 0)
	{
		ft_printf("TestPrint cd command old path :\t%s\n", current_working_directory);
		execute_cd(current_working_directory, minishell.all_commands.full_command);
		// current_working_directory = ft_calloc(sizeof(char), PATH_MAX);					// Comment out for debug
		// getcwd(current_working_directory, PATH_MAX);										// Comment out for debug
		// ft_printf("TestPrint cd command new path :\t%s\n\n", current_working_directory);	// Comment out for debug
	}
	else if(str_comp(minishell.all_commands.full_command[0], "pwd") == 0)
	{
		execute_pwd(current_working_directory);
	}
	else if(str_comp(minishell.all_commands.full_command[0], "export") == 0)
	{
		// execute_env(minishell);															// Comment out for debug
		// write(1, "\n\n", 2);																// Comment out for debug
		execute_export(minishell);
		// execute_env(minishell);															// Comment out for debug
	}
	else if(str_comp(minishell.all_commands.full_command[0], "unset") == 0)
	{
		// execute_env(minishell);															// Comment out for debug
		// write(1, "\n\n", 2);																// Comment out for debug
		execute_unset(minishell);
		// execute_env(minishell);															// Comment out for debug
	}
	else if(str_comp(minishell.all_commands.full_command[0], "env") == 0)
	{
		execute_env(minishell);
	}
	else if(str_comp(minishell.all_commands.full_command[0], "exit") == 0)
	{
		free(current_working_directory);
		execute_exit(minishell);
	}
	free(current_working_directory);
}
// Subject : "echo with option -n"
// Check if string has been placed in a char* , or if each word is a char* -> VERY PROBABLY - Check w/ Leonid
void	execute_echo(char **command_array)
{
	// the string to print is very probably made of many strings. Loop through command_array and print till the last element
	// print till next node = null
	if(str_comp(command_array[1], "-n") == 0)
	{
		ft_printf("%s", command_array[2]);
	}
	else
	{
		ft_printf("%s\n", command_array[1]);
	}
}

// Subject : "cd with only a relative or absolute path"
void	execute_cd(char *current_working_directory, char **command_array)
{
	if(chdir(command_array[1]) == -1)
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
	new_environment_variable = create_new_environment_variable(minishell.all_commands.full_command[1], minishell.all_commands.full_command[3]);
	add_env_var_to_list(minishell.env_variables, new_environment_variable);
}

// Subject : "unset with no options"
void	execute_unset(t_shell minishell)
{
	delete_env_var(minishell.env_variables, minishell.all_commands.full_command[1], del_string);
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
void	execute_exit(t_shell minishell)
{
	free_everything(minishell);
	exit(0);
}
