#include "minishell.h"
#include "libft.h"

void	execute_external_commands(t_shell *minishell)
{
	t_command	*all_commands = minishell->pipeline->cmds;
	char		*current_command;
	size_t	i = 0;
	int		fd[2] = {0, 1};			// fd[0] = in --- fd[1] = out

	char	**execve_args;						// Tableau de strings (arguments du programme executé par execve)
	execve_args = ft_calloc(sizeof(char *), 4);	// Room for 4 args
	if (!execve_args)
	{
		perror("");								// Switch arg to NULL ?
	}

	int		execve_args_count = 0;
	// Option for later : construire argv dans une boucle (mais requiert une realloc a chaque ajout TBC)

	int	commands_left = minishell->pipeline->count;

	while(commands_left > 0)									// 🟣 |				ls | grep sources		sort < y | > z
	{
		execve_args_count = 0;
		current_command = all_commands->argv[0];
		execve_args[execve_args_count] = ft_strjoin("/bin/", current_command);				// Create binary path for the ext command
		execve_args_count++;
		i++;
		while (all_commands->argv[i])														// (If applicable) Flag(s)
		{
			execve_args[execve_args_count] = ft_strdup(all_commands->argv[i]);
			execve_args_count++;
			i++;
		}
		if(all_commands->infile)							// 🟣 <				wc -l < doc		sort < doc
		{
			fd[0] = fetch_fd(all_commands->infile, false, false);
		}
		if(all_commands->outfile)							// 🟣 >				ls > doc
		{
			if (all_commands->append == 1)					// 🟣 >>			ls >> doc
			{
				fd[1] = fetch_fd(all_commands->outfile, true, false);
			}
			else
			{
				fd[1] = fetch_fd(all_commands->outfile, false, true);
			}
		}
		if(all_commands->has_heredoc == 1)					// 🟣 <<			grep ok << fin
		{
			// all input has been typed (but not saved ?)
			// delimiter saved in struct
			// Leo is working on it, wait till he's done
			return;
		}
		if(minishell->pipeline->count > 1)
		{
			// use pipe somewhere around here

		}
		fork_and_exec(fd, execve_args);
		commands_left--;
		all_commands++;
	}
}

void	fork_and_exec(int *fd, char	**execve_args)
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
		printf("%sDEBUG * Param FD_in : %d - Param FD_out : %d - Command : %s\n%s", CYAN, fd[0], fd[1], execve_args[0], NC);
		if(fd[0] != STDIN_FILENO)
		{
			printf("%sNew infile - FD %d is now FD 0 *** Stdout theorically unchanged : %d\n%s", YELLOW, fd[0], fd[1], NC);
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);					// Ok to close because it's been duplicated and it's now 0
		}
		if(fd[1] != STDOUT_FILENO)
		{
			printf("%sNew outfile - FD %d is now FD 1 *** Stdin theorically unchanged : %d\n%s", YELLOW, fd[1], fd[0], NC);
			dup2(fd[1], STDOUT_FILENO);
			// From here, nothing will printed on monitor because FD has changed
			close(fd[1]);					// Ok to close because it's been duplicated and it's now 1
		}
		printf("%s", GREEN);	// So that the official output stands out
		fflush(0);				// Remove after debug
		if (execve(execve_args[0], execve_args, envp) == -1)
		{
			perror("------------------ Error");
		}
		printf("%s", NC);
		// No need to revert FD back to normal as everything is happening only within the child
	}
}

int		fetch_fd(char *file_name, bool append, bool truncate)
{
	int		fd;
	char	*file_path = NULL;
	file_path = build_path(file_name);
	if(append)
	{
		fd = open(file_path, O_CREAT | O_APPEND | O_RDWR, 0666);	// HYPER IMPORTANT - Tout se joue dans les flags - 0666 = permissions
	}
	else if (truncate)
	{
		fd = open(file_path, O_CREAT | O_TRUNC | O_RDWR, 0666);		// HYPER IMPORTANT - Tout se joue dans les flags - 0666 = permissions
	}
	else
	{
		fd = open(file_path, O_CREAT | O_RDWR, 0666);		// HYPER IMPORTANT - Tout se joue dans les flags - 0666 = permissions
	}
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
