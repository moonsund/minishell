#include "minishell.h"
#include "libft.h"

void	replace_cmd_by_binary_path(char *cmd);
char	*build_path(char *file_name);
int		open_fd(char *file_name, bool append, bool truncate);
void	fd_update_if_redirections(t_command *all_commands, int *fd);
void	close_and_set_to_neg(int *fd);

void	replace_cmd_by_binary_path(char *cmd)
{
	char	*first_arg;

	first_arg = ft_strjoin("/bin/", cmd);				// Create binary path for the current command
	free(cmd);
	cmd = ft_strdup(first_arg);							// So it keeps the same address, easier to track for debug
	free(first_arg);
}

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

int		open_fd(char *file_name, bool append, bool truncate)
{
	int		fd;
	char	*file_path;

	file_path = build_path(file_name);
	// HYPER IMPORTANT - Tout se joue dans les flags - 0666 = permissions
	if(append)
	{
		fd = open(file_path, O_CREAT | O_APPEND | O_RDWR, 0666);
	}
	else if (truncate)
	{
		fd = open(file_path, O_CREAT | O_TRUNC | O_RDWR, 0666);
	}
	else
	{
		fd = open(file_path, O_CREAT | O_RDWR, 0666);
	}
	free(file_path);
	if (fd == -1)
	{
		perror("Error");
	}
	return(fd);
}

void	fd_update_if_redirections(t_command *all_commands, int *fd)
{
	if (all_commands->infile)							// 🟣 <	<<			wc -l < doc		sort < doc		grep ok << fin
		fd[0] = open_fd(all_commands->infile, false, false);
	if (all_commands->outfile)							// 🟣 >				ls > doc
	{
		if (all_commands->append == 1)					// 🟣 >>			ls >> doc
			fd[1] = open_fd(all_commands->outfile, true, false);
		else
			fd[1] = open_fd(all_commands->outfile, false, true);
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
