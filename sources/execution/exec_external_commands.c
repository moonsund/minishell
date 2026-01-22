#include "minishell.h"
#include "libft.h"

void	execute_external_commands(t_shell *minishell);
void	pipes_party(t_shell *minishell, int *fd_in, int *fd_out, char **execve_args);
void	fork_and_exec(t_shell *minishell, int *fd, char **execve_args);

void	execute_external_commands(t_shell *minishell)
{
	t_command	*all_commands = minishell->pipeline->cmds;
	int			backup_stdout;
	int			backup_stdin;
	int			fd[2] = {STDIN_FILENO, STDOUT_FILENO};		// fd[0] = in --- fd[1] = out
	int			pipe_fd_in[2] = {STDIN_FILENO, -1};			// Ssi appel de pipe : fd[0] = read - fd[1] = write
	int			pipe_fd_out[2] = {-1, -1};					// Ssi appel de pipe : fd[0] = read - fd[1] = write
	// char		*debug_string;
	// debug_string = ft_calloc(sizeof(char), 101);

	char	**execve_args;									// Commande qui sera executée par execve (avec flags, si présents)
	int	commands_left = minishell->pipeline->count;

	while(commands_left > 0)
	{
		execve_args = all_commands->argv;
		// attention, si la redirection est apres un pipe, on ne doit pas lire depuis stdin
		// if (execve_args[0][0] != '>' && execve_args[0][0] != '<')		// Switch to this after Leo's fix
		if (minishell->tokens.head->raw_str[0] != '>' && minishell->tokens.head->raw_str[0] != '<')
		{
			replace_cmd_by_binary_path(execve_args[0]);
		}
		fd_update_if_redirections(all_commands, fd);

		if (minishell->pipeline->count == 1)
		{
			if((ft_strcmp(minishell->tokens.head->raw_str, ">") == 0) || (ft_strcmp(minishell->tokens.head->raw_str, ">>") == 0))
				add_user_input_to_fd(minishell, fd[1]);
			else
				fork_and_exec(minishell, fd, execve_args);		// No pipes = keep things easy - at least for now
			return;
		}
		/* Pipes - Test commands :
		ls | grep sources | wc
		cat ok | wc -l | >> w
		cat w | cat -e | > w
		*/
		else
		{
			if (minishell->pipeline->count == commands_left)	// To pass here only once
			{
				backup_stdout = dup(STDOUT_FILENO);
				backup_stdin = dup(STDIN_FILENO);
			}
			if (commands_left > 1)
			{
				if(pipe(pipe_fd_out) == -1)						// Attribue des nouveaux fd DISPOS au 2eme passage (can be a prev used one if closed)
					perror("Error");
			}
			else if (commands_left == 1)
			{
				pipe_fd_out[1] = backup_stdout;					// Pour afficher last output dans le terminal
			}
			// ATTENTION !!!!! Ne jamais fermer un fd qui n'a pas ete ouvert
			// fermer pipe_fd_in[0] ici pour que grep sache ou s'arreter, mais pas ici
			pipes_party(minishell, pipe_fd_in, pipe_fd_out, execve_args);
			close_and_set_to_neg(&pipe_fd_in[0]);					// Previously ouvert comme stdin
			close_and_set_to_neg(&pipe_fd_in[1]);					// Not necessary au premier passage car deja -1, a voir pour la suite
			pipe_fd_in[0] = pipe_fd_out[0];							// On recupere la read side du pipe pour le brancher en fd_in
			pipe_fd_in[1] = pipe_fd_out[1];							// Pas sure de ca, vu qu'on a pas besoin de pipe_fd_in[1]
			// read(pipe_fd_in[0], debug_string, 100);					// DEBUG - output de ls
			// close pipe_fd_out pour pouvoir le reutiliser au prochain passage ?
			// close_and_set_to_neg(&pipe_fd_out[0]);
			// close_and_set_to_neg(&pipe_fd_out[1]);
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
	close_and_set_to_neg(&backup_stdin);
	close_and_set_to_neg(&backup_stdout);
}

// ls | grep sources		sort < y | > z
// Considerer pipe_fd_in[0], pipe_fd_out[1]
void	pipes_party(t_shell *minishell, int *fd_in, int *fd_out, char **execve_args)
{
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
			close_and_set_to_neg(&fd_out[1]);	// Ok to close because it's been duplicated and it's now 1
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
	char	**envp;
	pid_t	fork_pid_return;

	envp = build_envp(&minishell->env_vars);
	fork_pid_return = fork();
	if(fork_pid_return == -1)
			perror("Error");
	// Child :
	if (fork_pid_return == 0)
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
	// Parent :
	if(waitpid(fork_pid_return, &minishell->exit_status, 0) == -1)	// Exit status update if error - Check in 'man waitpid' if issues
		perror("Error");
	if(minishell->exit_status != 0)
		perror("Exit status updated");
	// Close fd that have been open - If necessary TBC
	if(fd[0] != STDIN_FILENO)
		close_and_set_to_neg(&fd[0]);
	if(fd[1] != STDOUT_FILENO)
		close_and_set_to_neg(&fd[1]);
}
