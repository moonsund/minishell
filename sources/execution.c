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

#include "minishell.h"

void	execute_command(t_shell commands/*, TBD */)
{

}
