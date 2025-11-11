#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

void	execute_built_in_commands(t_shell minishell/*, TBD */)
{
	char	*current_working_directory;
	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);
	getcwd(current_working_directory, PATH_MAX);

	if(ft_memcmp(minishell.all_commands.full_command[0], "echo", 5) == 0)
	{
		execute_echo(minishell.all_commands.full_command);
	}
	else if(ft_memcmp(minishell.all_commands.full_command[0], "cd", 3) == 0)
	{
		printf("TestPrint cd command old path :\t%s\n", current_working_directory);
		execute_cd(current_working_directory, minishell.all_commands.full_command);
		current_working_directory = ft_calloc(sizeof(char), PATH_MAX);			// Verif ✅
		getcwd(current_working_directory, PATH_MAX);							// Verif ✅
		printf("TestPrint cd command new path :\t%s\n\n", current_working_directory);
	}
	else if(ft_memcmp(minishell.all_commands.full_command[0], "pwd", 4) == 0)
	{
		execute_pwd(current_working_directory);									// Verif ✅
	}
	else if(ft_memcmp(minishell.all_commands.full_command[0], "export", 7) == 0)
	{
		execute_export(minishell);
	}
	else if(ft_memcmp(minishell.all_commands.full_command[0], "unset", 6) == 0)
	{
		execute_unset(minishell);
	}
	else if(ft_memcmp(minishell.all_commands.full_command[0], "env", 4) == 0)
	{
		execute_env(minishell);
	}
	else if(ft_memcmp(minishell.all_commands.full_command[0], "exit", 5) == 0)
	{
		execute_exit(minishell);
	}
}
// Subject : "echo with option -n"
// Check if string has been placed in a char* , or if each word is a char* -> VERY PROBABLY - Check w/ Leonid
void	execute_echo(char **command_array)
{
	// the string to print is very probably made of many strings. Loop through command_array and print till the last element
	if(ft_memcmp(command_array[1], "-n", 3) == 0)
	{
		printf("%s", command_array[2]);
	}
	else
	{
		printf("%s\n", command_array[1]);
	}
}

// Subject : "cd with only a relative or absolute path"
void	execute_cd(char *current_working_directory, char **command_array)
{
	char	*updated_path;
	char	*path_section;
	size_t	size_to_crop;
	size_t	size_new_path;

	updated_path = command_array[1];

	// if(ft_memcmp(command_array[1], "/", 1) == 0)								// Absolute path
	// {
	// 	updated_path = command_array[1];
	// }
	// else if(ft_memcmp(command_array[1], ".", 2) == 0)							// Check 2 bytes to make sure there's a \0 after the .
	// {
	// 	updated_path = current_working_directory;
	// }
	// else if(ft_memcmp(command_array[1], "..", 3) == 0)							// Check in 3 bytes to make sure there's a \0 after the ..
	// {
	// 	path_section = ft_strrchr(current_working_directory, '/');				// Searches the last occurence of '/'
	// 	size_to_crop = strlen(path_section);
	// 	ft_memset(path_section, 0, size_to_crop);
	// 	updated_path = current_working_directory;
	// }
	// else if(ft_memcmp(command_array[1], "./", 2) == 0)							// Relative path (same folder)
	// {
	// 	path_section = ft_strchr(command_array[1], '/');						// Ptr to the first occurence of '/'
	// 	updated_path = ft_strjoin(current_working_directory, path_section);		// Ⓜ️
	// }
	// else if(ft_memcmp(command_array[1], "../", 2) == 0)							// Relative path (parent folder)
	// {
	// 	path_section = ft_strrchr(current_working_directory, '/');				// Ptr to the last occurence of '/'
	// 	size_to_crop = strlen(path_section);
	// 	ft_memset(path_section, 0, size_to_crop);
	// 	path_section = ft_strchr(command_array[1], '/');						// Ptr to the first occurence of '/'
	// 	updated_path = ft_strjoin(current_working_directory, path_section);		// Ⓜ️
	// }
	// else																		// Relative path but without ./ (classic simple cd)
	// {
	// 	path_section = ft_strjoin(current_working_directory, "/");				// Ⓜ️
	// 	updated_path = ft_strjoin(path_section, command_array[1]);				// Ⓜ️
	// 	free(path_section);
	// }
	// if forgotten edge case in the 'else', add an else with the following error message
	// printf("Whoops - Command not recognized / Out of Minishell requirements\n");
	// free + return;
	if(chdir(updated_path) == -1)
	{
		free(updated_path);
		perror("Error");											// Errno prints the rest of the message
	}
	free(updated_path);
}

// Subject : "pwd with no options"
void	execute_pwd(char *current_working_directory)
{
	printf("%s\n", current_working_directory);
}

// Subject : "export with no options"
// no options = no flags (TBC) - So I have to code export ENV_VAR_NAME="Data in variable to add or edit"
void	execute_export(t_shell minishell)
{
	// export = créer ou modifier variable d’environnement
	// export ENV_VAR_NAME="Data in variable to add or edit"
	// export with no options : print all env var that have been exported
}

// Subject : "unset with no options"
void	execute_unset(t_shell minishell)
{
	// In shell scripting, unset is a built-in command used to delete shell variables or functions.
	// When you use unset on a variable, it removes the variable from the shell environment, making it undefined.
}
// Subject : "env with no options or arguments"
void	execute_env(t_shell minishell)
{
	// loop through all var and print them with '=' in between
}

// Subject : "exit with no options"
void	execute_exit(t_shell minishell)
{
	// Free memory (pass command_array) / Call a clean up function
	exit(0);
}
