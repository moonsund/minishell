#include "minishell.h"
#include "libft.h"

char		*build_path(char *file_name);
int			open_fd(char *file_name, bool append, bool truncate);
// void		fd_update_if_redirections(t_command *all_commands, int *fd_in, int *fd_out);
void		open_and_close_fd(t_command *cmd);

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

int	open_fd(char *file_name, bool append, bool truncate)
{
	int		fd;
	char	*file_path;

	file_path = build_path(file_name);
	// HYPER IMPORTANT - Tout se joue dans les flags - 0644 = permissions
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

// void	fd_update_if_redirections(t_command *all_commands, int *fd_in, int *fd_out)
// {
// 	// R_IN / R_OUT / R_APPEND / R_HEREDOC
// 	if (all_commands->redirs->type == R_IN)
// 		fd_in[0] = open_fd(all_commands->redirs->target, false, false);
// 	else if (all_commands->redirs->type == R_OUT)
// 		fd_out[0] = open_fd(all_commands->redirs->target, false, true);
// 	else if (all_commands->redirs->type == R_APPEND)
// 		fd_out[0] = open_fd(all_commands->redirs->target, true, false);
// }

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

	// while (true)					// Not required - Could create issues if kept ?
	// {
	// 	line = readline(NULL);
	// 	if (g_sigint) // ctrl+c - Not functional yet
	// 	{
	// 		close(fd);
	// 		return ;
	// 	}
	// 	write_line_in_fd(fd, line);
	// 	free(line);
	// }
}
