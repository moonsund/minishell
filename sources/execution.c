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

void	execute_command(t_shell command/*, TBD */)
{
	if(ft_memcmp(command.command_array[0], "pwd", 3) == 0)
	{
		char	*getcwd_return;
		char	*buf;
		buf = malloc(sizeof(char) * 50);
		size_t	z = 50;

		getcwd_return = getcwd(buf, z);
		printf("%s\n", getcwd_return);
		free(buf);
	}
}
// Only for testing this file (use current file config in debugger)
int		main(void)
{
	t_shell	command;
	command.command_array = malloc(sizeof(char*) * 4);		// Faire de la place pour 4 potentielles commandes
	command.command_array[0] = ft_strdup("pwd");			// Malloc done in strdup

	execute_command(command);

	return(0);
}
