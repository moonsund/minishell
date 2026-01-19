#include "minishell.h"
#include "libft.h"

void	replace_cmd_by_binary_path(char *cmd)
{
	char	*first_arg;

	first_arg = ft_strjoin("/bin/", cmd);				// Create binary path for the current command
	free(cmd);
	cmd = ft_strdup(first_arg);							// So it keeps the same address, easier to track for debug
	free(first_arg);
}

void	fd_update_if_redirections(t_command *all_commands, int *fd)
{
	if(all_commands->infile)							// 🟣 <	<<			wc -l < doc		sort < doc		grep ok << fin
		fd[0] = fetch_fd(all_commands->infile, false, false);
	if(all_commands->outfile)							// 🟣 >				ls > doc
	{
		if (all_commands->append == 1)					// 🟣 >>			ls >> doc
			fd[1] = fetch_fd(all_commands->outfile, true, false);
		else
			fd[1] = fetch_fd(all_commands->outfile, false, true);
	}
	// if(all_commands->has_heredoc == 1)				// 🟣 << No need to do anything, same behaviour as < w/ infile
	// {
	// 	// delimiter saved in struct
	// 	// input saved in .heredoc_0 (struct infile)
	// 	// shell behavior handled already
	// }
}

void	execute_external_commands(t_shell *minishell)
{
	t_command	*all_commands = minishell->pipeline->cmds;
	char	**envp = build_envp(&minishell->env_vars);
	int		fd[2] = {STDIN_FILENO, STDOUT_FILENO};		// fd[0] = in --- fd[1] = out
	int		pipe_fd_in[2] = {STDIN_FILENO, -1};			// fd[0] = read - fd[1] = write
	int		pipe_fd_out[2] = {-1, -1};					// fd[0] = read - fd[1] = write
	int		backup_stdout = dup(STDOUT_FILENO);			// Penser à le fermer
	char	*pour_test;
	pour_test = ft_calloc(sizeof(char), 101);

	char	**execve_args;								// Commande qui sera executée par execve (avec flags, si présents)
	int	commands_left = minishell->pipeline->count;

	while(commands_left > 0)								// 🟣 |				ls | grep sources		sort < y | > z
	{
		execve_args = all_commands->argv;
		replace_cmd_by_binary_path(execve_args[0]);
		fd_update_if_redirections(all_commands, fd);

		if(minishell->pipeline->count == 1)					// No pipes = keep things easy - at least for now
			fork_and_exec(minishell, envp, fd, execve_args);
		else												// command :	ls | grep sources
		{
			// ouvrir pipe_fd_out
			if(pipe(pipe_fd_out) == -1)
				perror("Error");
			if(commands_left == 1)
				fd[1] = backup_stdout;						// reset fd[1] à sa valeur initiale pour afficher last output dans le terminal

			// dup2(pipe_fd[1], fd[1]);						// fd[1] (out) pointe maintenant sur pipe_fd[1] (write)

			fork_and_exec(minishell, envp, {pipe_fd_in[0], pipe_fd_out[1]}, execve_args);	// solution pour boucle - Manip de pipes a faire avant
			close(pipe_fd_in[0] + [1]) // a faire a partir du 2eme passage
			pipe_fd_in = pipe_fd_out

			// conditions
			// premier passage dans la boucle = pipe_fd_in doit etre STDIN
			// dernier passage = pipe_fd_out = STDOUT

			fork_and_exec(minishell, envp, {STDIN_FILENO, pipe_fd1[1]}, execve_args);	// ls
			pipe(pipe_fd2);
			fork_and_exec(minishell, envp, {pipe_fd1[0], pipe_fd2[1]}, execve_args);		// grep

			fork_and_exec(minishell, envp, {pipe_fd2[0], STDOUT_FILENO}, execve_args);	// cmd3
			// read(pipe_fd[0], pour_test, 100);				// output de ls - ALLELUHIA
			// fd[0] = pipe_fd[0];			// update de input fd[0] pour qu'il soit pipe_fd[0] dans la commande suivante (fork?)
		}
		commands_left--;
		all_commands++;
	}
	// if(ft_strcmp(minishell->pipeline->cmds->infile, ".heredoc_0") == 0)			// Test & Comment out when the rest is functional
	// {
	// 	char	*heredoc_file = build_path(".heredoc_0");
	// 	unlink(heredoc_file);
	// 	free(heredoc_file);
	// }
}

// Fork always needed, otherwise the program quits right after execution of 1st command
void	fork_and_exec(t_shell *minishell, char	**envp, int *fd, char	**execve_args)
{
	pid_t	fork_pid_return = fork();
	if(fork_pid_return == -1)
			perror("Error");
	pid_t child_pid;

	if(fork_pid_return != 0)										// Le parent attend le résultat avec waitpid
	{
		// close fd_in a tester (refer to pipe_testing file)
		child_pid = fork_pid_return;
		if(waitpid(child_pid, &minishell->exit_status, 0) == -1)	// Exit status update if error - Check in 'man waitpid' if issues
			perror("Error");
		if(minishell->exit_status != 0)
			perror("Exit status updated");
	}
	else		// Le child exécute la commande binaire avec execve
	{
		if(fd[0] != STDIN_FILENO)
		{
			// printf("%sNew infile - FD %d is now FD 0 *** Stdout theorically unchanged : %d\n%s", YELLOW, fd[0], fd[1], NC);
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);					// Ok to close because it's been duplicated and it's now 0
		}
		if(fd[1] != STDOUT_FILENO)
		{
			// printf("%sNew outfile - FD %d is now FD 1 *** Stdin theorically unchanged : %d\n%s", YELLOW, fd[1], fd[0], NC);
			dup2(fd[1], STDOUT_FILENO);
			// From here, nothing will printed on monitor because FD has changed
			close(fd[1]);					// Ok to close because it's been duplicated and it's now 1
		}
		if (execve(execve_args[0], execve_args, envp) == -1)
		{
			perror("Error");
		}
		// No need to revert FD back to normal as everything is happening only within the child
	}
}

int		fetch_fd(char *file_name, bool append, bool truncate)
{
	int		fd;
	char	*file_path = NULL;
	file_path = build_path(file_name);
	// HYPER IMPORTANT - Tout se joue dans les flags - 0666 = permissions
	if(append)
	{
		fd = open(file_path, O_CREAT | O_APPEND | O_RDWR, 0666);
	}
	else if (truncate)
	{
		fd = open(file_path, O_CREAT | O_TRUNC | O_RDWR, 0666);
	}
	else
	{
		fd = open(file_path, O_CREAT | O_RDWR, 0666);
	}
	free(file_path);
	if (fd == -1)
	{
		perror("Error");
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
