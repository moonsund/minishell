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
void	execute_pwd(char *current_working_directory);
void	execute_cd(char *current_working_directory, char **command_array);

void	execute_commands(t_shell command/*, TBD */)
{
	char	*current_working_directory;
	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);
	getcwd(current_working_directory, PATH_MAX);

	if(ft_memcmp(command.command_array[0], "pwd", 3) == 0)
	{
		execute_pwd(current_working_directory);									// Verif ✅
	}
	if(ft_memcmp(command.command_array[0], "cd", 2) == 0)
	{
		execute_cd(current_working_directory, command.command_array);
		current_working_directory = ft_calloc(sizeof(char), PATH_MAX);			// Verif ✅
		getcwd(current_working_directory, PATH_MAX);							// Verif ✅
	}
}

void	execute_pwd(char *current_working_directory)
{
	printf("%s\n", current_working_directory);
}

// Subject : "cd with only a relative or absolute path"
void	execute_cd(char *current_working_directory, char **command_array)
{
	char	*updated_path;
	char	*path_section;
	size_t	size_to_crop;
	size_t	size_new_path;

	if(ft_memcmp(command_array[1], ".", 2) == 0)						// Check in 2 bytes to make sure there's a \0 after the .
	{
		// Only one . = Do nothing = Display the command invite
		// ou chdir current_working_directory ? Comme ca on le met juste une fois a la fin
		// updated_path = current_working_directory
		// remove return line in that case
		return;
	}
	else if(ft_memcmp(command_array[1], "..", 3) == 0)					// Check in 3 bytes to make sure there's a \0 after the ..
	{
		path_section = ft_strrchr(current_working_directory, '/');		// Searches the last occurence of '/'
		size_to_crop = strlen(path_section);
		size_new_path = strlen(current_working_directory) - size_to_crop;
		updated_path = ft_calloc(sizeof(char), size_new_path + 1);
		updated_path = ft_memmove(updated_path, current_working_directory, size_new_path);
		if(chdir(updated_path) == -1)
		{
			perror("Error");											// Errno prints the rest of the message
		}
	}
	else if(ft_memcmp(command_array[1], "./", 2) == 0)					// Relative path (same folder)
	{
		path_section = ft_strtrim(command_array[1], ".");								// Ⓜ️
		updated_path = ft_strjoin(current_working_directory, path_section);				// Ⓜ️
		if(chdir(updated_path) == -1)
		{
			perror("Error");
		}
	}
	else if(ft_memcmp(command_array[1], "../", 2) == 0)					// Relative path (parent folder)
	{
		// Crop the last directory
		// cat paths
		path_section = ft_strrchr(current_working_directory, '/');		// Ptr to the last occurence of '/'
		size_to_crop = strlen(path_section);
		ft_memset(path_section, 0, size_to_crop);						// Do same for .. ?
		updated_path = ft_calloc(sizeof(char), PATH_MAX);
		// DO DO, check from here - WIP
		updated_path = ft_memmove(updated_path, current_working_directory, size_new_path);
		// same till here
		// remove .. from command_array[1]
		// join + chdir
	}
	else if(ft_memcmp(command_array[1], "/", 1) == 0)					// Absolute path
	{
		if(chdir(command_array[1]) == -1)
		{
			perror("Error");
		}
	}
	else
	{
		// ?? Error message : Command not recognized / Out of Minishell requirements
		// return;
	}
}

// Only for testing this file (use current file config in debugger)
int		main(void)
{
	t_shell	command;
	command.command_array = malloc(sizeof(char*) * 4);				// Faire de la place pour 4 potentielles commandes
	// command.command_array[0] = ft_strdup("pwd");						// 🟡 Test pwd
	command.command_array[0] = ft_strdup("cd");						// 🟡 Test cd
	// command.command_array[1] = ft_strdup(".");						// 🟡 Test cd .
	// command.command_array[1] = ft_strdup("..");						// 🟡 Test cd ..
	// command.command_array[1] = ft_strdup("./test_folder");			// 🟡 Test cd + relative path
	command.command_array[1] = ft_strdup("/home/schappuy/00_Root");			// 🟡 Test cd + absolute path

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
