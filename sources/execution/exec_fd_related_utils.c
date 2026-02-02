/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_fd_related_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:18:36 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/02 20:18:37 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

char	*build_path(char *file_name);
int		open_fd(char *file_name, bool append, bool truncate);
void	open_and_close_fd(t_command *cmd);

char	*build_path(char *file_name)
{
	char	*current_working_directory;
	char	*cwd_with_slash;
	char	*file_path;

	current_working_directory = fetch_current_working_directory();
	cwd_with_slash = ft_strjoin(current_working_directory, "/");
	file_path = ft_strjoin(cwd_with_slash, file_name);
	free(current_working_directory);
	free(cwd_with_slash);
	return (file_path);
}

// Flag 0644 = permissions
int	open_fd(char *file_name, bool append, bool truncate)
{
	int		fd;
	char	*file_path;

	file_path = build_path(file_name);
	if (append)
	{
		fd = open(file_path, O_CREAT | O_APPEND | O_RDWR, 0644);
	}
	else if (truncate)
	{
		fd = open(file_path, O_CREAT | O_TRUNC | O_RDWR, 0644);
	}
	else
	{
		fd = open(file_path, O_CREAT | O_RDWR, 0644);
	}
	free(file_path);
	if (fd == -1)
	{
		perror("Error");
	}
	return (fd);
}

void	open_and_close_fd(t_command *cmd)
{
	int		fd;
	char	*line;

	if (cmd->redirs->type == R_OUT)
		fd = open_fd(cmd->redirs->target, false, true);
	else if (cmd->redirs->type == R_APPEND)
		fd = open_fd(cmd->redirs->target, true, false);
	else
		return ;
	close(fd);
}
