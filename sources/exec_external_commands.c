#include "minishell.h"
#include "libft.h"

void	execute_external_commands(t_shell *minishell)
{
	// t_token	*all_commands;
	// all_commands = minishell->tokens.head;
	t_command	*all_commands = minishell->pipeline->cmds;
	char		*current_command;
	size_t	i = 0;
	int		fd_in = 0;				// Create fd[] array instead
	int		fd_out = 1;

	bool	cmd_binary_found = false;

	char	**execve_args;						// Tableau de strings (arguments du programme executé par execve)
	execve_args = ft_calloc(sizeof(char *), 4);	// Room for 4 args
	if (!execve_args)
	{
		perror("");								// Switch arg to NULL ?
	}

	int		execve_args_count = 0;
	// Option for later : construire argv dans une boucle (mais requiert une realloc a chaque ajout TBC)

	while (minishell->pipeline->cmds->argv[i])
	{
		if (!cmd_binary_found)
		{
			current_command = minishell->pipeline->cmds->argv[0];
			execve_args[execve_args_count] = ft_strjoin("/bin/", current_command);				// Create binary path for the ext command
			cmd_binary_found = true;															// Maybe useless ?
			execve_args_count++;
			i++;
		}
		if (all_commands->argv[i])
		{
			execve_args[execve_args_count] = ft_strdup(all_commands->argv[i]);				// (If applicable) Flag
			execve_args_count++;
		}
		if(all_commands->infile)					// 🟣 <				wc -l < doc		sort < doc
		{
			fd_in = fetch_fd(all_commands->infile);
			execve_args_count++;
		}
		if(all_commands->outfile)					// 🟣 >				ls > doc
		{
			fd_out = fetch_fd(all_commands->outfile);
			execve_args_count++;
		}
		// if(all_commands->has_heredoc == TOK_HEREDOC) {}		// 🟣 <<
		// if(all_commands->append) {}							// 🟣 >>
		// if(???) {}											// 🟣 |
		i++;
	}
	fork_and_exec(fd_in, fd_out, execve_args);
}
// ls > test.txt
void	fork_and_exec(int fd_stdin, int fd_stdout, char	**execve_args)
{
	char	*envp[] = {NULL};								// Fetch from struct using build_envp function (Done by Leo TBC)
	pid_t	fork_pid_return = fork();						// Seulement utile en cas de < << > >> |
	if(fork_pid_return == -1)
			perror("------------------ Error");
	pid_t child_pid;
	int status;												// Status code = the one to put in the env var for error return ?

	if(fork_pid_return != 0)								// Le parent attend le résultat avec waitpid
	{
		child_pid = fork_pid_return;
		if(waitpid(child_pid, &status, 0) == -1)
			perror("------------------ Error");
		if(status != 0)
			printf("status is not 0 (%d) - Check macro in 'man waitpid' to find out what that means\n", status);
		printf("%sDEBUG * From Parent - PID : %d\n%s", BLUE, getpid(), NC);
	}
	else		// Le child exécute la commande binaire avec execve
	{
		printf("%sDEBUG * From Child - PID : %d - Parent PID : %d\n%s", CYAN, getpid(), getppid(), NC);
		printf("%sDEBUG * Param FD_in : %d - Param FD_out : %d - Command : %s\n%s", CYAN, fd_stdin, fd_stdout, execve_args[0], NC);
		printf("%s", GREEN);
		fflush(0);				// Remove after debug
		if(fd_stdin != STDIN_FILENO)
		{
			printf("%sNew infile - FD %d is now FD 0\n%s", YELLOW, fd_stdin, NC);
			dup2(fd_stdin, STDOUT_FILENO);
			close(fd_stdin);					// Ok to close because it's been duplicated and it's now 0
		}
		if(fd_stdout != STDOUT_FILENO)
		{
			printf("%sNew outfile - FD %d is now FD 1\n%s", YELLOW, fd_stdout, NC);
			dup2(fd_stdout, STDOUT_FILENO);
			// From here, nothing will printed on monitor because FD has changed
			close(fd_stdout);					// Ok to close because it's been duplicated and it's now 1
		}
		if (execve(execve_args[0], execve_args, envp) == -1)
		{
			perror("------------------ Error");
		}
		printf("%s", NC);
		// No need to revert FD back to normal as everything is happening only within the child
	}
}

// ls > test.txt
int		fetch_fd(char *file_name)
{
	int		fd;
	char	*file_path = NULL;
	file_path = build_path(file_name);
	fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);					// HYPER IMPORTANT - Tout se joue ici
	free(file_path);
	if (fd == -1)
	{
		perror("------------------ Error");
	}
	return(fd);
}

char	*build_path(char *file_name)
{
	char	*current_working_directory = fetch_current_working_directory();
	char	*cwd_with_slash = ft_strjoin(current_working_directory, "/");
	char	*file_path = ft_strjoin(cwd_with_slash, file_name);
	free(current_working_directory);
	free(cwd_with_slash);
	return (file_path);
}
