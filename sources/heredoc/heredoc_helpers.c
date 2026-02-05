/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:09 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 12:13:02 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void			redir_replace_with_infile(t_redir *r, char *filename);
int				write_line_in_fd(int fd, char *line);
int				append_charter(char **line, char c);
int				append_string(char **line, const char *str);
t_exit_status	hd_abort(int fd, char *filename, t_exit_status st);

void	redir_replace_with_infile(t_redir *r, char *filename)
{
	free(r->target);
	r->target = filename;
	r->type = R_IN;
	r->fd = 0;
	r->expand = 0;
}

int	write_line_in_fd(int fd, char *line)
{
	ssize_t	bw;
	size_t	len;

	if (!line)
		return (write(fd, "\n", 1) >= 0);
	len = ft_strlen(line);
	if (len > 0)
	{
		bw = write(fd, line, len);
		if (bw < 0)
			return (0);
	}
	return (write(fd, "\n", 1) >= 0);
}

int	append_charter(char **line, char c)
{
	char	*new_line;
	size_t	len;

	if (!line)
		return (0);
	if (!*line)
	{
		new_line = malloc(2);
		if (!new_line)
			return (0);
		new_line[0] = c;
		new_line[1] = '\0';
		return (*line = new_line, 1);
	}
	len = ft_strlen(*line);
	new_line = malloc(len + 2);
	if (!new_line)
		return (0);
	ft_memcpy(new_line, *line, len);
	new_line[len] = c;
	new_line[len + 1] = '\0';
	free(*line);
	*line = new_line;
	return (1);
}

int	append_string(char **line, const char *str)
{
	size_t	l1;
	size_t	l2;
	char	*new_line;

	l2 = ft_strlen(str);
	if (!*line)
	{
		new_line = malloc(l2 + 1);
		if (!new_line)
			return (0);
		ft_memcpy(new_line, str, l2);
		new_line[l2] = '\0';
		return (*line = new_line, 1);
	}
	l1 = ft_strlen(*line);
	new_line = malloc(l1 + l2 + 1);
	if (!new_line)
		return (0);
	ft_memcpy(new_line, *line, l1);
	ft_memcpy(new_line + l1, str, l2);
	new_line[l1 + l2] = '\0';
	free(*line);
	*line = new_line;
	return (1);
}

t_exit_status	hd_abort(int fd, char *filename, t_exit_status st)
{
	if (fd >= 0)
		close(fd);
	if (filename)
	{
		unlink(filename);
		free(filename);
	}
	setup_signals();
	return (st);
}
