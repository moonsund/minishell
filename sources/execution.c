/*

NOTES :

- Work with struct from parsing. This struct will provide :
	- Args (first command splitted in command + flags)
	- Potentionally an infile/outfile
	- Pointer to the next node ( = next command)
- Final output from parsing : a linked list (1 node = 1 command)
- Example : ```ls -la | grep .c | wc -l``` = 3 nodes

TASK LIST :

- Create loop
- Execute command(s) from the linked list (in the right order)
- Deal w/ Pipes (also if many in a row) & Redirections
- Wait for next linked list (= input from stdin)
- Deal w/ Signals
- Deal w/ Paths
- Wait for all child processes to go up the loop
- Exit input = leave loop

- Notions to study :
	- 2 kinds of execution : Builtin (in Shell = No fork) VS. Extern (Child Process)
	- fork (shell cloning - parent/child)
	- dup2 (duplicates a fd)
	- execve (replaces process by another one / launches a command)
	- Pipes (output from left = input for right - first command writes in pipe / second command reads from pipe)
		- To create a pipe : pipe(fd)
	- Env variables

*/

// #include "minishell.h"

// For Quick Debug
#include "../includes/minishell.h"
#include "libft.h"

void	execute_commands(t_shell command/*, TBD */);
void	execute_echo(char **command_array);
void	execute_cd(char *current_working_directory, char **command_array);
void	execute_pwd(char *current_working_directory);

void	execute_commands(t_shell command/*, TBD */)
{
	char	*current_working_directory;
	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);
	getcwd(current_working_directory, PATH_MAX);

	if(ft_memcmp(command.command_array[0], "echo", 5) == 0)
	{
		execute_echo(command.command_array);
	}
	else if(ft_memcmp(command.command_array[0], "cd", 3) == 0)
	{
		execute_cd(current_working_directory, command.command_array);
		current_working_directory = ft_calloc(sizeof(char), PATH_MAX);			// Verif ✅
		getcwd(current_working_directory, PATH_MAX);							// Verif ✅
	}
	else if(ft_memcmp(command.command_array[0], "pwd", 4) == 0)
	{
		execute_pwd(current_working_directory);									// Verif ✅
	}
}
// Subject : "echo with option -n"
void	execute_echo(char **command_array)
{
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

	if(ft_memcmp(command_array[1], "/", 1) == 0)						// Absolute path
	{
		updated_path = command_array[1];
	}
	else if(ft_memcmp(command_array[1], ".", 2) == 0)					// Check 2 bytes to make sure there's a \0 after the .
	{
		updated_path = current_working_directory;
	}
	else if(ft_memcmp(command_array[1], "..", 3) == 0)					// Check in 3 bytes to make sure there's a \0 after the ..
	{
		path_section = ft_strrchr(current_working_directory, '/');		// Searches the last occurence of '/'
		size_to_crop = strlen(path_section);
		ft_memset(path_section, 0, size_to_crop);
		updated_path = current_working_directory;
	}
	else if(ft_memcmp(command_array[1], "./", 2) == 0)					// Relative path (same folder)
	{
		path_section = ft_strchr(command_array[1], '/');						// Ptr to the first occurence of '/'
		updated_path = ft_strjoin(current_working_directory, path_section);		// Ⓜ️
	}
	else if(ft_memcmp(command_array[1], "../", 2) == 0)						// Relative path (parent folder)
	{
		path_section = ft_strrchr(current_working_directory, '/');			// Ptr to the last occurence of '/'
		size_to_crop = strlen(path_section);
		ft_memset(path_section, 0, size_to_crop);
		path_section = ft_strchr(command_array[1], '/');					// Ptr to the first occurence of '/'
		updated_path = ft_strjoin(current_working_directory, path_section);	// Ⓜ️
	}
	else
	{
		printf("Whoops - Command not recognized / Out of Minishell requirements\n");
		return;
	}
	if(chdir(updated_path) == -1)
	{
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
void	execute_export()
{
	// export ENV_VAR_NAME="Data in variable to add or edit"
}

// NEXT
// Subject : "unset with no options"
// Subject : "env with no options or arguments"
// Subject : "exit with no options"

// Only for testing this file (use current file config in debugger)
int		main(void)
{
	t_shell	command;
	command.command_array = malloc(sizeof(char*) * 4);						// Faire de la place pour 4 potentielles commandes
	// command.command_array[0] = ft_strdup("pwd");							// 🟡 Test pwd
	// command.command_array[0] = ft_strdup("cd");							// 🟡 Test cd
	// command.command_array[1] = ft_strdup(".");							// 🟡 Test cd .
	// command.command_array[1] = ft_strdup("..");							// 🟡 Test cd ..
	// command.command_array[1] = ft_strdup("../objects");					// 🟡 Test cd + relative path
	// command.command_array[1] = ft_strdup("/home/schappuy/00_Root");		// 🟡 Test cd + absolute path
	command.command_array[0] = ft_strdup("echo");							// 🟡 Test echo
	command.command_array[1] = ft_strdup("-n");								// 🟡 Test echo
	command.command_array[2] = ft_strdup("OMG");							// 🟡 Test echo

	execute_commands(command);

	return(0);
}

/*
// Learn & Fix
int main() {
	char	*ppath = "ls";
	char	*argv;
	char	*envp;

	printf("Finished reading\n");
	execve("ls", NULL, NULL);
	printf("OK\n");					// Ne sera pas affiché car ls a remplacé le binaire précédent
    return 0;
}

*/
