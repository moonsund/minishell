#include "minishell.h"
#include "libft.h"

void	execute_built_in_commands(t_shell minishell/*, TBD */);
void	execute_echo(char **command_array);
void	execute_cd(char *current_working_directory, char **command_array);
void	execute_pwd(char *current_working_directory);
void	execute_export(t_shell minishell);
void	execute_unset(t_shell minishell);
void	execute_env(t_shell minishell);
void	execute_exit(t_shell minishell);
void	create_env(t_env *env_variables);

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
	// Find where these variables are stored + call GNL in a loop to print all lines ?
	// Or create them from scratch ?
	char *env_ret;
	env_ret = getenv(NULL);		// Test
	printf("%s\n", env_ret);
}

// Subject : "exit with no options"
void	execute_exit(t_shell minishell)
{
	// Free memory (pass command_array) / Call a clean up function
	exit(0);
}

// liste chainee - A convertir en char ** pour execve sinon pas pratique
// Implémenter $ later (voir avec Leo qui s'en occupe)
void	create_env(t_env *env_variables)
{
	t_env	*head;
	t_env	*var_1;
	t_env	*var_2;
	head = create_new_environment_variable("USER", getenv("USER"));
	var_1 = create_new_environment_variable("LANG", getenv("LANG"));
	var_2 = create_new_environment_variable("HOME", getenv("HOME"));

	// TO DO
}

t_env	*create_new_environment_variable(char *key, char *value)
{
	t_env	*new_env_var;
	if (!key || !value)
		return (NULL);
	new_env_var = malloc(sizeof(t_env));
	if (!new_env_var)
		return (NULL);
	new_env_var->variable_name = key;
	new_env_var->variable_data = value;
	new_env_var->next = NULL;
	return (new_env_var);
}

t_env	*ft_lstlast(t_env *lst)
{
	t_env	*last;

	// Creating a ptr to the node I'll return (aka last one)
	if (!lst)
		return (NULL);
	// Giving 'last' the same address as the head node,
	// so it starts the loop at the right place
	last = lst;
	// Leaving the loop at the node before the last node
	while (last->next != NULL)
	{
		last = last->next;
	}
	// When 'last.next' is NULL, means 'last' is the last node,
	// so I return a pointer to it
	return (last);
}

void	ft_lstadd_back(t_env **lst, t_env *new)
{
	t_env	*last;

	if (!lst || !new)
		return ;
	if (!*lst)
	{
		*lst = new;
		// new->next = NULL; // NOPE, because 'new' may not be the last node
		return ;
	}
	last = ft_lstlast(*lst);
	last->next = new;
	// new->next = NULL; // NOPE, because 'new' may not be the last node
}

void	ft_lstdelone(t_env *lst, void (*del)(void *))
{
	if (!lst || !del)
		return ;
	// Free the pointer to the node, as it's been malloc'd in ft_lstnew
	if (lst)
	{
		//  Fetch the data to erase : `lst->content` (lst is a pointer,
		// so I use the -> instead of the .)
		del(lst->content);
		free(lst);
		//  Adding this to avoid dangling pointer
		lst = NULL;
		// this is actually useless, because it doesn't affect the original
		// pointer (because I'm changing a local copy of it).
		// A double ptr would work though
	}
}

// del function takes any kind of data as a parameter, and frees this data
void	del(void *param)
{
	if (param)
		free(param);
}
