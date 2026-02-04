/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:13 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 12:50:18 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status	heredoc_cleanup_return(int fd, char *filename,
					t_exit_status st);
void			redir_replace_with_infile(t_redir *r, char *filename);
char			*generate_heredoc_filename(size_t heredoc_index);
t_exit_status	write_line_in_fd(int fd, const char *line);

t_exit_status	heredoc_cleanup_return(int fd, char *filename, t_exit_status st)
{
	if (fd >= 0)
		close(fd);
	if (filename)
	{
		unlink(filename);
		free(filename);
	}
	return (st);
}

void	redir_replace_with_infile(t_redir *r, char *filename)
{
	free(r->target);
	r->target = filename;
	r->type = R_IN;
	r->fd = 0;
	r->expand = 0;
}

char	*generate_heredoc_filename(size_t heredoc_index)
{
	char	*file_index;
	char	*file_name;

	file_index = ft_itoa(heredoc_index);
	if (!file_index)
		return (NULL);
	file_name = ft_strjoin(".heredoc_", file_index);
	free(file_index);
	return (file_name);
}

t_exit_status	write_line_in_fd(int fd, const char *line)
{
	ssize_t	written;
	size_t	len;
	size_t	off;

	if (!line)
		line = "";
	len = ft_strlen(line);
	off = 0;
	while (off < len)
	{
		written = write(fd, line + off, len - off);
		if (written < 0)
			return (ES_GENERAL);
		off += (size_t)written;
	}
	if (write(fd, "\n", 1) < 0)
		return (ES_GENERAL);
	return (ES_SUCCESS);
}
