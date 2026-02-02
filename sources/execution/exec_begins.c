/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_begins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:31 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/02 23:16:27 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			execute_pipeline(t_shell *shell);
int			exec_pipeline_forking(t_shell *shell, const t_pipeline *pl);
static void	apply_redirs_or_die(const t_command *cmd);
static int	open_redir_file(const t_redir *redir);
static int	wait_all_and_get_last(pid_t *pids, size_t count);

/*
Auto check Valgrind VSCode : Ctrl Shift p > run task > Valgrind
Redirections - Test commands :
wc -l < w
sort < infile
grep ok << end
pwd > outfile			----- LEAK
ls >> outfile
export FRUIT=apple > new_outfile
cat w y z > new

Pipes - Test commands :
ls | grep sources | wc
ls -la | grep git | wc -l
cat z | sort | head -5
ls | exit | wc
pwd | grep z | wc -m
echo London | cat -e > y

*/

int	execute_pipeline(t_shell *shell)
{
	t_pipeline	*pl;
	t_command	*cmd;

	pl = shell->pipeline;
	if (pl->count == 0 || !pl->cmds)
		return (ES_GENERAL);
	cmd = &pl->cmds[0];
	// cd/export/unset/exit with and w/o redirections = all commands that don't print anything but modify the shell
	if (pl->count == 1 && cmd->argv && cmd->argv[0] && is_parent_builtin(cmd->argv[0]))
		return (exec_builtin_in_parent(shell, cmd));

	// Command line starting with a redirection, but no pipe, no cmd and no other redirection, e.g. '> outfile'
	if ((shell->pipeline->count == 1) && (!shell->pipeline->cmds->argv)
		&& shell->pipeline->cmds->redirs
		&& (shell->pipeline->cmds->redirs->type == R_OUT
			|| shell->pipeline->cmds->redirs->type == R_APPEND))
	{
		open_and_close_fd(cmd);
		return (0);
	}
	// only redirections, builtins and external commands both with and w/o redirections
	return (exec_pipeline_forking(shell, pl));
}

// only redirections, builtins and external commands both with and w/o redirections
int	exec_pipeline_forking(t_shell *shell, const t_pipeline *pl)
{
	size_t	i;
	int		prev_read;
	int		pipefds[2];
	pid_t	*pids;
	pid_t	pid;
	int		last_status;

	if (!pl || pl->count == 0)
		return (0);
	pids = (pid_t *)malloc(sizeof(pid_t) * pl->count);
	if (!pids)
		return (1);
	i = 0;
	prev_read = -1;
	while (pl->count > i) // true while we have commands for the execution
	{
		pipefds[0] = -1;       // read end
		pipefds[1] = -1;       // write end
		if (i + 1 < pl->count) // checks if we need a pipe for the current command
		{
			if (pipe(pipefds) < 0)
			{
				perror("pipe");
				free(pids);
				return (1);
			}
		}
		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			close_if_valid(pipefds[0]);
			close_if_valid(pipefds[1]);
			close_if_valid(prev_read);
			free(pids);
			return (1);
		}
		// TESTING : echo z > w | echo ok
		if (pid == 0) // child
		{
			if (prev_read != -1) // if not the 1st pipe
			{
				if (dup2(prev_read, STDIN_FILENO) < 0) // dup2(old, new)
				{
					perror("dup2 stdin");
					exit(1);
				}
			}
			// if not the last command
			if (pipefds[1] != -1)
			{
				if (dup2(pipefds[1], STDOUT_FILENO) < 0)
				{
					perror("dup2 stdout");
					exit(1);
				}
			}
			close_if_valid(prev_read);
			close_if_valid(pipefds[0]);
			close_if_valid(pipefds[1]);
			// apply redirs
			apply_redirs_or_die(&pl->cmds[i]);
			// command line with only redirections, e.g. "< in > out"
			if (!pl->cmds[i].argv || !pl->cmds[i].argv[0])
				exit(0);
			// in-pipes redirections without command 'ls | > y' - Nothing to do
			// if ((!pl->cmds[i].argv) && (pl->cmds[i].redirs))
			// 	exit (0);
			if (is_builtin(pl->cmds[i].argv[0]))
			{
				last_status = run_any_builtin_in_child(shell, &pl->cmds[i]);
				exit(last_status);
			}
			else
			{
				if (!execute_external_commands(shell, &pl->cmds[i]))
				{
					perror("execve");
					if (errno == ENOENT) // No such file or directory
						exit(127);
					else
						exit(126); // EACCES, EISDIR, ENOEXEC, etc.
				}
			}
			// NB: the child MUST ALWAYS terminate with exit(status)
		}
		// parent
		pids[i] = pid;
		close_if_valid(prev_read);
		close_if_valid(pipefds[1]);
		prev_read = pipefds[0];
		i++;
	}
	/*
	exit status of the WHOLE pipeline = exit status of the LAST command in the pipeline: ls | grep x | wc -l
	When we fork a pipeline:
		we know the pid of each segment
		we know the pid of the last command
		we wait for ALL pids
		but we take the exit status only from last_pid
	Hence, the parent:
		- waits for all
		- returns the status of the last command in the pipeline
	*/
	close_if_valid(prev_read);
	last_status = wait_all_and_get_last(pids, pl->count);
	free(pids);
	return (last_status);
}

static void	apply_redirs_or_die(const t_command *cmd)
{
	t_redir	*redir;
	int		fd;

	redir = cmd->redirs;
	while (redir)
	{
		fd = open_redir_file(redir);
		if (fd < 0)
		{
			perror(redir->target);
			exit(1);
		}
		if (dup2(fd, redir->fd) < 0)
		{
			perror("dup2");
			close(fd);
			exit(1);
		}
		close(fd);
		redir = redir->next;
	}
}

static int	open_redir_file(const t_redir *redir)
{
	int	fd;

	fd = -1;
	if (redir->type == R_IN)
		fd = open(redir->target, O_RDONLY);
	else if (redir->type == R_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == R_APPEND)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redir->type == R_HEREDOC)
		fd = open(redir->target, O_RDONLY);
	return (fd);
}

static int	wait_all_and_get_last(pid_t *pids, size_t count)
{
	size_t	i;
	int		status;
	int		last_status;
	pid_t	last_pid;

	last_status = 0;
	last_pid = pids[count - 1];

	i = 0;
	while (i < count)
	{
		if (waitpid(pids[i], &status, 0) > 0)
		{
			if (pids[i] == last_pid)
			{
				if (WIFEXITED(status))
					last_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					last_status = 128 + WTERMSIG(status);
				else
					last_status = 1;
			}
		}
		i++;
	}
	return (last_status);
}
