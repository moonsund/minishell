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
		fd[0] = open_fd(all_commands->infile, false, false);
	if(all_commands->outfile)							// 🟣 >				ls > doc
	{
		if (all_commands->append == 1)					// 🟣 >>			ls >> doc
			fd[1] = open_fd(all_commands->outfile, true, false);
		else
			fd[1] = open_fd(all_commands->outfile, false, true);
	}
	// if(all_commands->has_heredoc == 1)				// 🟣 << No need to do anything, same behaviour as < w/ infile
	// {
	// 	// delimiter saved in struct
	// 	// input saved in .heredoc_0 (struct infile)
	// 	// shell behavior handled already
	// }
}

void	close_and_set_to_neg(int *fd)
{
	if (*fd && *fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}

void	execute_external_commands(t_shell *minishell)
{
	t_command	*all_commands = minishell->pipeline->cmds;
	int			backup_stdout = dup(STDOUT_FILENO);			// Penser à le fermer
	int			backup_stdin = dup(STDIN_FILENO);			// Penser à le fermer
	int			fd[2] = {STDIN_FILENO, STDOUT_FILENO};		// fd[0] = in --- fd[1] = out
	int			pipe_fd_in[2] = {STDIN_FILENO, -1};			// Ssi appel de pipe : fd[0] = read - fd[1] = write
	int			pipe_fd_out[2] = {-1, -1};					// Ssi appel de pipe : fd[0] = read - fd[1] = write
	char		*pour_test;
	pour_test = ft_calloc(sizeof(char), 101);

	char	**execve_args;								// Commande qui sera executée par execve (avec flags, si présents)
	int	commands_left = minishell->pipeline->count;

	while(commands_left > 0)
	{
		execve_args = all_commands->argv;
		replace_cmd_by_binary_path(execve_args[0]);
		fd_update_if_redirections(all_commands, fd);

		if(minishell->pipeline->count == 1)					// No pipes = keep things easy - at least for now
			fork_and_exec(minishell, fd, execve_args);
		else												// command :	ls | grep sources
		{
			if (commands_left > 1)
			{
				if(pipe(pipe_fd_out) == -1)						// Attribue des nouveaux fd dispos au 2eme passage
					perror("Error");
			}
			else if (commands_left == 1)
			{
				pipe_fd_out[1] = backup_stdout;						// Pour afficher last output dans le terminal
			}
			// ATTENTION !!!!! Ne jamais fermer un fd qui n'a pas ete ouvert
			// fermer pipe_fd_in[0] ici pour que grep sache ou s'arreter, mais pas ici car n'a
			pipes_party(minishell, pipe_fd_in, pipe_fd_out, execve_args);
			close_and_set_to_neg(&pipe_fd_in[0]);					// Previously ouvert comme stdin
			close_and_set_to_neg(&pipe_fd_in[1]);					// Not necessary au premier passage car deja -1, a voir pour la suite
			pipe_fd_in[0] = pipe_fd_out[0];							// On recupere la read side du pipe pour le brancher en fd_in
			pipe_fd_in[1] = pipe_fd_out[1];							// Pas sure de ca, vu qu'on a pas besoin de pipe_fd_in[1]
			// read(pipe_fd_in[0], pour_test, 100);					// DEBUG - output de ls
			// close_and_set_to_neg(&pipe_fd_out[0]);
			// close_and_set_to_neg(&pipe_fd_out[1]);
			// close pipe_fd_out pour pouvoir le reutiliser au prochain passage ?
			// Restaurer sdtin car fucked up

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

// ls | grep sources		sort < y | > z
void	pipes_party(t_shell *minishell, int *fd_in, int *fd_out, char **execve_args)
{
	// Considerer pipe_fd_in[0], pipe_fd_out[1]
	char	**envp = build_envp(&minishell->env_vars);
	pid_t	fork_pid_return = fork();
	if(fork_pid_return == -1)
			perror("Error");
	pid_t child_pid;

	if(fork_pid_return != 0)										// Le parent attend le résultat avec waitpid
	{
		close_and_set_to_neg(&fd_in[0]);							// fermer fd input pour que grep sache ou s'arreter
		close(fd_out[1]);
		child_pid = fork_pid_return;
		if(waitpid(child_pid, &minishell->exit_status, 0) == -1)	// Exit status update if error - Check in 'man waitpid' if issues
			perror("Error");
		if(minishell->exit_status != 0)
			perror("Exit status updated");
	}
	else		// Le child exécute la commande binaire avec execve
	{
		if(fd_in[0] != STDIN_FILENO)			// A revoir ? NB : 0 peut etre ensuite automatiquement attribué à un autre fd
		{
			dup2(fd_in[0], STDIN_FILENO);
			close_and_set_to_neg(&fd_in[0]);	// Ok to close because it's been duplicated and it's now 0
		}
		if(fd_out[1] != STDOUT_FILENO)			// A revoir ? NB : 1 peut etre ensuite automatiquement attribué à un autre fd
		{
			dup2(fd_out[1], STDOUT_FILENO);
			// From here, nothing will printed on monitor because stdout has changed
			close_and_set_to_neg(&fd_out[1]);	// Ok to close because it's been duplicated and it's now 0
		}
		if (execve(execve_args[0], execve_args, envp) == -1)
		{
			perror("Error");
		}
		// No need to revert FD back to normal as everything is happening only within the child
	}
}


// Fork always needed, otherwise the program quits right after execution of 1st command
void	fork_and_exec(t_shell *minishell, int *fd, char **execve_args)
{
	char	**envp = build_envp(&minishell->env_vars);
	pid_t	fork_pid_return = fork();
	if(fork_pid_return == -1)
			perror("Error");
	pid_t child_pid;

	if(fork_pid_return != 0)										// Le parent attend le résultat avec waitpid
	{
		child_pid = fork_pid_return;
		if(waitpid(child_pid, &minishell->exit_status, 0) == -1)	// Exit status update if error - Check in 'man waitpid' if issues
			perror("Error");
		if(minishell->exit_status != 0)
			perror("Exit status updated");
	}
	else		// Le child exécute la commande binaire avec execve
	{
		if(fd[0] != STDIN_FILENO)			// NB : Si passage après pipe(), 0 peut etre ensuite automatiquement attribué à un autre fd
		{
			dup2(fd[0], STDIN_FILENO);
			close_and_set_to_neg(&fd[0]);	// Ok to close because it's been duplicated and it's now 0
		}
		if(fd[1] != STDOUT_FILENO)			// NB : Si passage après pipe(), 1 peut etre ensuite automatiquement attribué à un autre fd
		{
			dup2(fd[1], STDOUT_FILENO);
			// From here, nothing will printed on monitor because stdout has changed
			close_and_set_to_neg(&fd[1]);	// Ok to close because it's been duplicated and it's now 0
		}
		if (execve(execve_args[0], execve_args, envp) == -1)
		{
			perror("Error");
		}
		// No need to revert FD back to normal as everything is happening only within the child
	}
}

int		open_fd(char *file_name, bool append, bool truncate)
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
