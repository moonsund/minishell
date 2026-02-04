/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_begins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:31 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 12:44:15 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			execute_pipeline(t_shell *shell);
int			exec_pipeline_forking(t_shell *shell, const t_pipeline *pl);
static void	apply_redirs_or_die(const t_command *cmd);
static int	open_redir_file(const t_redir *redir);
static int	wait_all_and_get_last(pid_t *pids, size_t count);

int	execute_pipeline(t_shell *shell)
{
	t_pipeline	*pl;
	t_command	*cmd;

	pl = shell->pipeline;
	if (pl->count == 0 || !pl->cmds)
		return (ES_GENERAL);
	cmd = &pl->cmds[0];
	if (pl->count == 1 && cmd->argv && cmd->argv[0] && is_parent_builtin(cmd->argv[0]))
		return (exec_builtin_in_parent(shell, cmd));
	if ((shell->pipeline->count == 1) && (!shell->pipeline->cmds->argv)
		&& shell->pipeline->cmds->redirs
		&& (shell->pipeline->cmds->redirs->type == R_OUT
			|| shell->pipeline->cmds->redirs->type == R_APPEND))
	{
		open_and_close_fd(cmd);
		return (0);
	}
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
	while (pl->count > i)
	{
		pipefds[0] = -1;
		pipefds[1] = -1;
		if (i + 1 < pl->count)	// checks if we need a pipe for the current command
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
		if (pid == 0)
		{
			if (prev_read != -1) // if not the 1st pipe
			{
				if (dup2(prev_read, STDIN_FILENO) < 0)
				{
					perror("dup2 stdin");
					exit(1);
				}
			}
			if (pipefds[1] != -1) // if not the last command
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
			apply_redirs_or_die(&pl->cmds[i]);
			if (!pl->cmds[i].argv || !pl->cmds[i].argv[0])
				exit(0);
			if (is_builtin(pl->cmds[i].argv[0]))
			{
				last_status = run_any_builtin_in_child(shell, &pl->cmds[i]);
				exit(last_status);
			}
			else
			{
				if (execute_external_commands(shell, &pl->cmds[i]) > 0)
				{
					if (errno == ENOENT) // No such file or directory
						exit(127);
					else
						exit(126); // EACCES, EISDIR, ENOEXEC, etc.
				}
			}
		}
		pids[i] = pid;
		close_if_valid(prev_read);
		close_if_valid(pipefds[1]);
		prev_read = pipefds[0];
		i++;
	}
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
