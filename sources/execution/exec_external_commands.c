#include "minishell.h"
#include "libft.h"

void	execute_external_commands(t_shell *minishell);
void	fork_and_exec(t_shell *minishell, int *fd_in, int *fd_out, char **execve_args);
void	parent_process_actions(t_shell *minishell, int fork_pid_return, int *fd_in, int *fd_out);
void	child_process_actions(char **execve_args, char **envp, int *fd_in, int *fd_out);

void	execute_external_commands(t_shell *minishell)
{
	t_command	*all_commands = minishell->pipeline->cmds;
	int			backup_stdout;
	int			backup_stdin;
	int			fd_in[2] = {STDIN_FILENO, -1};			// Ssi appel de pipe : fd[0] = read - fd[1] = write
	int			fd_out[2] = {STDOUT_FILENO, -1};		// Ssi appel de pipe : fd[0] = read - fd[1] = write
	// char		*debug_string;
	// debug_string = ft_calloc(sizeof(char), 101);

	char	**execve_args;
	int	commands_left = minishell->pipeline->count;

	while(commands_left > 0)
	{
		if (all_commands->argv)
		{
			execve_args = all_commands->argv;
			replace_cmd_by_binary_path(execve_args[0]);
		}
		else
		{
			if((minishell->pipeline->count == 1) &&
					minishell->pipeline->cmds->redirs &&
						(minishell->pipeline->cmds->redirs->type == R_OUT ||
							minishell->pipeline->cmds->redirs->type == R_APPEND))
				add_user_input_to_fd(minishell, fd_out[0]);
		}
		// attention, si la redirection est apres un pipe, on ne doit pas lire depuis stdin
		if (minishell->pipeline->cmds->redirs)
			fd_update_if_redirections(all_commands, fd_in, fd_out);

		// if (minishell->pipeline->count == 1)
		// {
		// 	// Tous les else/if sont a revoir - Trouver une logique qui marche

		// 	else
		// 		fork_and_exec(minishell, fd_in, fd_out, execve_args);		// No pipes = keep things easy - at least for now
		// 	return;
		// }
/* Pipes - Test commands :
ls | grep sources | wc
ls -la | grep ob | wc -l
cat main.c | sort | head -5

cat w | wc -l | >> z
cat w | cat -e | > z
sort z | wc | > w
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
				if(pipe(fd_out) == -1)						// Attribue des nouveaux fd DISPOS au 2eme passage (can be a prev used one if closed)
					perror("Error");						// Attention, a attribue le meme numero que fd_in[1] pour fd_out[0] - Voir si soucis
			}
			else if (commands_left == 1)
			{
				fd_out[1] = backup_stdout;					// Pour afficher last output dans le terminal
			}
			// ATTENTION !!!!! Ne jamais fermer un fd qui n'a pas ete ouvert
			fork_and_exec(minishell, fd_in, fd_out, execve_args);
			close_and_set_to_neg(&fd_in[0]);					// Previously ouvert comme stdin - Won't close because == 0
			fd_in[0] = fd_out[0];							// On recupere la read side du pipe pour le brancher en fd_in
			fd_in[1] = fd_out[1];							// Pas sure de ca, vu qu'on a pas besoin de fd_in[1]
			// read(fd_in[0], debug_string, 100);			// DEBUG
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

// Considerer fd_in[0], fd_out[1]

// Fork always needed, otherwise the program quits right after execution of 1st command
void	fork_and_exec(t_shell *minishell, int *fd_in, int *fd_out, char **execve_args)
{
	char	**envp;
	pid_t	fork_pid_return;

	envp = build_envp(&minishell->env_vars);
	fork_pid_return = fork();
	if(fork_pid_return == -1)
			perror("Error");
	// Child
	if(fork_pid_return == 0)
		child_process_actions(execve_args, envp, fd_in, fd_out);
	// Parent
	parent_process_actions(minishell, fork_pid_return, fd_in, fd_out);
	free(envp);
}

void	child_process_actions(char **execve_args, char **envp, int *fd_in, int *fd_out)
{
	if(fd_in[0] != STDIN_FILENO)
	{
		dup2(fd_in[0], STDIN_FILENO);
		close_and_set_to_neg(&fd_in[0]);	// Infile fd has been duplicated and is now 0, so I can close the duplicated fd number
	}
	if (fd_out[1] == -1)					// Si c'est ce cas, on a pas ouvert de pipe et on a une seule commande
	{
		if(fd_out[0] != STDOUT_FILENO)
		{
			dup2(fd_out[0], STDOUT_FILENO);
			// From here, nothing will printed on monitor because stdout has changed
			close_and_set_to_neg(&fd_out[0]);	// Outfile fd has been duplicated and is now 0, so I can close the duplicated fd number
		}
	}
	else										// We're in a pipe
	{	// fermer la read-end du pipe fd_out car no need
		if(fd_out[1] != STDOUT_FILENO)			// 6 au lieu de backup stdout au dernier passage ? = not good
		{
			dup2(fd_out[1], STDOUT_FILENO);
			// From here, nothing will printed on monitor because stdout has changed
			close_and_set_to_neg(&fd_out[1]);
		}
	}
	if (execve(execve_args[0], execve_args, envp) == -1)
		perror("Error");
	// No need to revert FD back to normal as everything is happening only within the child
}

void	parent_process_actions(t_shell *minishell, int child_pid, int *fd_in, int *fd_out)
{
	if (minishell->pipeline->count > 1)
	{
		close_and_set_to_neg(&fd_in[0]);							// fermer fd input pour que grep sache ou s'arreter
		close(fd_out[1]);											// fermer write-side du pipe ouvert par le child car plus besoin
	}

	if(waitpid(child_pid, &minishell->exit_status, 0) == -1)		// Exit status update if error - Check in 'man waitpid' if issues
		perror("Error");
	if(minishell->exit_status != 0)
		printf("Exit status has just been updated to %d\n", minishell->exit_status);

	if (minishell->pipeline->count == 1)
	{
		// Close fd that have been open, if different than the standard ones.
		if(fd_in[0] != STDIN_FILENO)
			close_and_set_to_neg(&fd_in[0]);
		if(fd_out[0] != STDOUT_FILENO)
			close_and_set_to_neg(&fd_out[0]);
	}
}
