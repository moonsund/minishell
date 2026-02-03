/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_fd_related_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:36 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/03 13:35:43 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int			open_fd(const char *path, bool append, bool truncate);
void		open_and_close_fd(t_command *cmd);

// bool		is_permission_granted(const char *path, int fd);

// bool		is_permission_granted(const char *path, int fd)
// {
// 	/*
// 	int stat(const char *pathname, struct stat *statbuf);
// 	int fstat(int fd, struct stat *statbuf);
// 	int lstat(const char *pathname, struct stat *statbuf);

// 	   These  functions  return information about a file, in the buffer pointed to by stat‐
//        buf.  No permissions are required on the file itself, but—in the case of stat(), fs‐
//        tatat(),  and lstat()—execute (search) permission is required on all of the directo‐
//        ries in pathname that lead to the file.

//        stat() and fstatat() retrieve information about the file pointed to by pathname; the
//        differences for fstatat() are described below.

//        lstat()  is identical to stat(), except that if pathname is a symbolic link, then it
//        returns information about the link itself, not the file that the link refers to.

//        fstat() is identical to stat(), except that the file about which information  is  to
//        be retrieved is specified by the file descriptor fd.

// 	   Check return values
// 	*/

// }

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
		return;

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

