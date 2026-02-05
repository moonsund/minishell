/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_forking_helpers.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:31 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 12:05:05 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ctx_init(t_fork_ctx *ctx, pid_t *pids);
int		open_pipe_if_needed(t_fork_ctx *c, const t_pipeline *pl);
void	free_on_error(pid_t *pids, t_fork_ctx *c);
int		open_redir_file(const t_redir *redir);
void	apply_redirs_or_die(const t_command *cmd);

void	ctx_init(t_fork_ctx *ctx, pid_t *pids)
{
	ctx->i = 0;
	ctx->prev_read = -1;
	ctx->pipefds[0] = -1;
	ctx->pipefds[1] = -1;
	ctx->pids = pids;
}

void	free_on_error(pid_t *pids, t_fork_ctx *c)
{
	close_if_valid(c->prev_read);
	free(pids);
}

int	open_pipe_if_needed(t_fork_ctx *ctx, const t_pipeline *pl)
{
	ctx->pipefds[0] = -1;
	ctx->pipefds[1] = -1;
	if (ctx->i + 1 >= pl->count)
		return (0);
	if (pipe(ctx->pipefds) < 0)
	{
		perror("pipe");
		return (1);
	}
	return (0);
}

int	open_redir_file(const t_redir *redir)
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

void	apply_redirs_or_die(const t_command *cmd)
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
