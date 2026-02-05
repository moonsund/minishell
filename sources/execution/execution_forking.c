/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_forking.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:31 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 12:03:19 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			exec_pipeline_forking(t_shell *shell, const t_pipeline *pl);
static int	fork_one_segment(t_shell *sh, const t_pipeline *pl,
				t_fork_ctx *ctx);
static void	child_exec_segment(t_shell *sh, const t_command *cmd,
				t_fork_ctx *ctx);
static void	child_dup_or_die(int from, int to, const char *perr);
static int	wait_all_and_get_last(pid_t *pids, size_t count);

int	exec_pipeline_forking(t_shell *shell, const t_pipeline *pl)
{
	pid_t		*pids;
	t_fork_ctx	ctx;
	int			last_status;

	if (!pl || pl->count == 0)
		return (0);
	pids = (pid_t *)malloc(sizeof(*pids) * pl->count);
	if (!pids)
		return (1);
	ctx_init(&ctx, pids);
	while (ctx.i < pl->count)
	{
		if (fork_one_segment(shell, pl, &ctx))
			return (free_on_error(pids, &ctx), 1);
	}
	close_if_valid(ctx.prev_read);
	last_status = wait_all_and_get_last(pids, pl->count);
	free(pids);
	return (last_status);
}

static int	fork_one_segment(t_shell *sh, const t_pipeline *pl, t_fork_ctx *ctx)
{
	pid_t	pid;

	if (open_pipe_if_needed(ctx, pl))
		return (1);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		close_if_valid(ctx->pipefds[0]);
		close_if_valid(ctx->pipefds[1]);
		return (1);
	}
	if (pid == 0)
		child_exec_segment(sh, &pl->cmds[ctx->i], ctx);
	ctx->pids[ctx->i] = pid;
	close_if_valid(ctx->prev_read);
	close_if_valid(ctx->pipefds[1]);
	ctx->prev_read = ctx->pipefds[0];
	ctx->i++;
	return (0);
}

static void	child_exec_segment(t_shell *sh, const t_command *cmd,
		t_fork_ctx *ctx)
{
	int	status;

	child_dup_or_die(ctx->prev_read, STDIN_FILENO, "dup2 stdin");
	child_dup_or_die(ctx->pipefds[1], STDOUT_FILENO, "dup2 stdout");
	close_if_valid(ctx->prev_read);
	close_if_valid(ctx->pipefds[0]);
	close_if_valid(ctx->pipefds[1]);
	apply_redirs_or_die(cmd);
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	if (is_builtin(cmd->argv[0]))
	{
		status = run_any_builtin_in_child(sh, (t_command *)cmd);
		exit(status);
	}
	if (execute_external_commands(sh, (t_command *)cmd) > 0)
	{
		if (errno == ENOENT)
			exit(127);
		else
			exit(126);
	}
	exit(0);
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
