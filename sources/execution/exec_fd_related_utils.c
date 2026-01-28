#include "minishell.h"
#include "libft.h"

char	*build_path(char *file_name);
int		open_fd(char *file_name, bool append, bool truncate);
void	fd_update_if_redirections(t_command *all_commands, int *fd_in, int *fd_out);
void	add_user_input_to_fd(t_shell *minishell);
void	close_and_set_to_neg(int *fd);

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

void	fd_update_if_redirections(t_command *all_commands, int *fd_in, int *fd_out)
{
	// R_IN / R_OUT / R_APPEND / R_HEREDOC
	if (all_commands->redirs->type == R_IN)
		fd_in[0] = open_fd(all_commands->redirs->target, false, false);
	else if (all_commands->redirs->type == R_OUT)
		fd_out[0] = open_fd(all_commands->redirs->target, false, true);
	else if (all_commands->redirs->type == R_APPEND)
		fd_out[0] = open_fd(all_commands->redirs->target, true, false);
}

void	add_user_input_to_fd(t_shell *minishell)
{
	int		fd;
	char	*line;

	if (minishell->pipeline->cmds->redirs->type == R_OUT)						// Bash : Erase content if file exists / Create file if doesn't exist
		fd = open_fd(minishell->pipeline->cmds->redirs->target, false, true);
	else if (minishell->pipeline->cmds->redirs->type == R_APPEND)
		fd = open_fd(minishell->pipeline->cmds->redirs->target, true, false);
	else
		return ;

	while (true)
	{
		line = readline(NULL);
		if (g_sigint) // ctrl+c - Not functional yet
		{
			close(fd);
			return ;
		}
		write_line_in_fd(fd, line);
		free(line);
	}
}

void	close_and_set_to_neg(int *fd)
{
	if (*fd && *fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}
