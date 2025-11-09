/*		NOTES :

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
#include "libft.h"

void	execute_external_commands(t_shell minishell/*, TBD */);

/* Implement the following redirections:
		< should redirect input -- ‼️ PRIO1 faire avant pipes
		> should redirect output -- ‼️ PRIO2 faire avant pipes
		<< should be given a delimiter, then read the input until a line containing the delimiter is seen. However, it doesn’t have to update the history!
		>> should redirect output in append mode.
• Implement pipes (| character). The output of each command in the pipeline is connected to the input of the next command via a pipe.
• Handle environment variables ($ followed by a sequence of characters) which should expand to their values.
• Handle $? which should expand to the exit status of the most recently executed foreground pipeline. */

void	execute_external_commands(t_shell minishell/*, TBD */)
{
	// Fork / Pipe / FD
	// Find out how many nodes ?
	// Find out nodes types (pipes, redirections, )
	// Create child process = fork + execve
/*
if(command type == pipe)
{
	fork
}
*/

// if node command starts with 'ls'
	char **folder_content;
	char *path = ft_strdup("/home/schappuy/00_Root");
	folder_content = execute_ls(minishell, path);

}

char	**execute_ls(t_shell minishell, char *path)
{
	char			**content;
	DIR				*opendir_output_to_convert;
	struct dirent	*directory_content;
	opendir_output_to_convert = opendir(path);
	if(!opendir_output_to_convert)
	{
		// handle error (message + free mem)
		return(NULL);
	}

	content = malloc(sizeof(char*) * 100);
	int i = 0;
	directory_content = readdir(opendir_output_to_convert);
	while ((directory_content = readdir(opendir_output_to_convert)) != NULL)
	{
		content[i] = ft_strdup(directory_content->d_name);			// Ⓜ️
		i++;
	}
	content[i] = NULL;
	closedir(opendir_output_to_convert);
	return(content);
}
