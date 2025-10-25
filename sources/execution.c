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
void	execute_pwd();
void	execute_cd(char *requested_path);

void	execute_commands(t_shell command/*, TBD */)
{
	if(ft_memcmp(command.command_array[0], "pwd", 3) == 0)
	{
		execute_pwd();
	}
	if(ft_memcmp(command.command_array[0], "cd", 2) == 0)
	{
		execute_pwd(command.command_array[1]);
	}
}

void	execute_pwd()
{
	char	*getcwd_return;
	char	*buf;
	buf = malloc(sizeof(char) * 50);
	size_t	z = 50;

	getcwd_return = getcwd(buf, z);
	printf("%s\n", getcwd_return);
	free(buf);
}

// cd with only a relative or absolute path
void	execute_cd(char *requested_path)
{
	// Code the following options ? . (deal with ..) ~ cd (no args) -

	if(ft_memcmp(requested_path, ".", 1) == 0)
	{
		// Get current directory and remove everything after the last /
		if(chdir(/* Go up one dir */) == -1)
		{
			perror("Error");			// Errno prints the rest of the message
		}
	}
	else								// Path provided
	{
		if(chdir(requested_path) == -1)
		{
			perror("Error");			// Errno prints the rest of the message
		}
	}
}

// Only for testing this file (use current file config in debugger)
int		main(void)
{
	t_shell	command;
	command.command_array = malloc(sizeof(char*) * 4);				// Faire de la place pour 4 potentielles commandes
	command.command_array[0] = ft_strdup("cd");						// Malloc done in strdup
	command.command_array[1] = ft_strdup("./8_Minishell/");			// Malloc done in strdup

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
