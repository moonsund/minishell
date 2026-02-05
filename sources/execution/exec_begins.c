/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_begins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:31 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 10:22:25 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			execute_pipeline(t_shell *shell);
int			exec_pipeline_forking(t_shell *shell, const t_pipeline *pl);
static void	apply_redirs_or_die(const t_command *cmd);
static int	open_redir_file(const t_redir *redir);

int	execute_pipeline(t_shell *shell)
{
	t_pipeline	*pl;
	t_command	*cmd;

	pl = shell->pipeline;
	if (!pl || pl->count == 0 || !pl->cmds)
		return (0);
	cmd = &pl->cmds[0];
	if (pl->count == 1 && cmd->argv && cmd->argv[0]
		&& is_parent_builtin(cmd->argv[0]))
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

static void	ctx_init(t_fork_ctx *c, pid_t *pids)
{
	c->i = 0;
	c->prev_read = -1;
	c->pipefds[0] = -1;
	c->pipefds[1] = -1;
	c->pids = pids;
}

static void	child_dup_or_die(int from, int to, const char *perr)
{
	if (from == -1)
		return ;
	if (dup2(from, to) < 0)
	{
		perror(perr);
		exit(1);
	}
}

static void	child_exec_segment(t_shell *sh, const t_command *cmd, t_fork_ctx *c)
{
	int	st;

	child_dup_or_die(c->prev_read, STDIN_FILENO, "dup2 stdin");
	child_dup_or_die(c->pipefds[1], STDOUT_FILENO, "dup2 stdout");
	close_if_valid(c->prev_read);
	close_if_valid(c->pipefds[0]);
	close_if_valid(c->pipefds[1]);
	apply_redirs_or_die(cmd);
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	if (is_builtin(cmd->argv[0]))
	{
		st = run_any_builtin_in_child(sh, (t_command *)cmd);
		exit(st);
	}
	if (execute_external_commands(sh, (t_command *)cmd) > 0)
		exit((errno == ENOENT) ? 127 : 126);
	exit(0);
}

static int	open_pipe_if_needed(t_fork_ctx *c, const t_pipeline *pl)
{
	c->pipefds[0] = -1;
	c->pipefds[1] = -1;
	if (c->i + 1 >= pl->count)
		return (0);
	if (pipe(c->pipefds) < 0)
	{
		perror("pipe");
		return (1);
	}
	return (0);
}

static int	fork_one_segment(t_shell *sh, const t_pipeline *pl, t_fork_ctx *c)
{
	pid_t	pid;

	if (open_pipe_if_needed(c, pl))
		return (1);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		close_if_valid(c->pipefds[0]);
		close_if_valid(c->pipefds[1]);
		return (1);
	}
	if (pid == 0)
		child_exec_segment(sh, &pl->cmds[c->i], c);
	c->pids[c->i] = pid;
	close_if_valid(c->prev_read);
	close_if_valid(c->pipefds[1]);
	c->prev_read = c->pipefds[0];
	c->i++;
	return (0);
}

static void	free_on_error(pid_t *pids, t_fork_ctx *c)
{
	close_if_valid(c->prev_read);
	free(pids);
}

int	exec_pipeline_forking(t_shell *shell, const t_pipeline *pl)
{
	pid_t		*pids;
	t_fork_ctx	c;
	int			last_status;

	if (!pl || pl->count == 0)
		return (0);
	pids = (pid_t *)malloc(sizeof(*pids) * pl->count);
	if (!pids)
		return (1);
	ctx_init(&c, pids);
	while (c.i < pl->count)
	{
		if (fork_one_segment(shell, pl, &c))
			return (free_on_error(pids, &c), 1);
	}
	close_if_valid(c.prev_read);
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