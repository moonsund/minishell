/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_fd_related_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:36 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/03 20:31:19 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int		open_fd(const char *path, bool append, bool truncate);
void	open_and_close_fd(t_command *cmd);

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
