/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_fd_related_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:36 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/05 12:06:01 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int		open_fd(const char *path, bool append, bool truncate);
void	open_and_close_fd(t_command *cmd);
int		pipe_setup(int *pipefds, size_t *i, size_t pl_count, pid_t	*pids);
void	close_if_valid(int fd);
void	close_all_if_valid(int *fd, int *pipe_fd, bool exclude_read_pipe);

int	open_fd(const char *path, bool append, bool truncate)
{
	int	flags;
	int	fd;

	flags = O_CREAT;
	if (append)
		flags |= O_WRONLY | O_APPEND;
	else if (truncate)
		flags |= O_WRONLY | O_TRUNC;
	else
		flags |= O_WRONLY;
	fd = open(path, flags, 0644);
	if (fd < 0)
		perror(path);
	return (fd);
}

void	open_and_close_fd(t_command *cmd)
{
	t_redir	*r;
	int		fd;

	if (!cmd)
		return ;
	r = cmd->redirs;
	while (r)
	{
		fd = -1;
		if (r->type == R_OUT)
			fd = open_fd(r->target, false, true);
		else if (r->type == R_APPEND)
			fd = open_fd(r->target, true, false);
		if (fd >= 0)
			close(fd);
		r = r->next;
	}
}

int	pipe_setup(int *pipefds, size_t *i, size_t pl_count, pid_t *pids)
{
	pipefds[0] = -1;
	pipefds[1] = -1;
	if (*i + 1 < pl_count)
	{
		if (pipe(pipefds) < 0)
		{
			perror("pipe");
			free(pids);
			return (1);
		}
	}
	return (0);
}

// Ok to pass the fd rather than a pointer to it ? TBC
// Would it be smart to set to -1  after closing ? TBD
void	close_if_valid(int fd)
{
	if (fd >= 0)
		close(fd);
}

void	close_all_if_valid(int *fd, int *pipe_fd, bool exclude_read_pipe)
{
	if (*fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
	if (pipe_fd[1] >= 0)
	{
		close(pipe_fd[1]);
		pipe_fd[1] = -1;
	}
	if (!exclude_read_pipe)
	{
		if (pipe_fd[2] >= 0)
		{
			close(pipe_fd[0]);
			pipe_fd[0] = -1;
		}
	}
}
