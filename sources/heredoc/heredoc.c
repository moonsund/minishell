#include "minishell.h"

static char *generate_heredoc_filename(size_t heredoc_index);
// static char *expand_heredoc(char *line);
static int write_heredoc_line(int fd, char *line);

int process_heredocs(t_shell *shell)
{
	size_t  i;
	char    *line;
	int  fd;
	char *tmp_file_name;
	static size_t heredoc_index;

	heredoc_index = 0;

	if (!shell || !shell->pipeline)
		return (0);

	i = 0;
	while (i < shell->pipeline->count)
	{
		if (shell->pipeline->cmds[i].has_heredoc)
		{
			tmp_file_name = generate_heredoc_filename(heredoc_index++);
			if (!tmp_file_name)
				return (0);

			fd = open(tmp_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
			if (fd < 0)
			{
				free(tmp_file_name);
				return (0);
			}

			while (true)
			{
				line = readline("heredoc> ");
				if (!line)
				{
					close(fd);
					return (0);
				}

				if (shell->pipeline->cmds[i].heredoc_limiter
					&& ft_strcmp(line, shell->pipeline->cmds[i].heredoc_limiter) == 0)
				{
					free(line);
					break;
				}

				if (!is_empty(line))
					add_history(line);

				// здесь можно будет вставить expand_heredoc_line(), если нужно
				// if (shell->pipeline->cmds[i].heredoc_expand_needed)
				//     expand_heredoc(line);

				if (!write_heredoc_line(fd, line))
				{
					free(line);
					close(fd);
					return (0);
				}

				free(line);
			}
			close(fd);
		}
		i++;
	}
	return (1);
}

static char *generate_heredoc_filename(size_t heredoc_index)
{
	char *file_number;
	char *file_name;

	file_number = ft_itoa(heredoc_index);
	if (!file_number)
		return (NULL);
	file_name = ft_strjoin(".heredoc_", file_number);
	free (file_number);
	return (file_name);
}

// char *expand_heredoc(char *line)
// {

//     size_t i;
//     char *new_line;
//     int in_var;

//     i = 0;
//     while (line[i])
//     {
//         if (line[i] != '$' && !in_var)
//             append_char(new_line, line[i], NULL);
//         if (line[i] == '$' )




//     }









// }

static int write_heredoc_line(int fd, char *line)
{
	ssize_t bytes_written;
	size_t len;

	if (!line)
		return (1);

	len = ft_strlen(line);
	if (len > 0)
	{
		bytes_written = write(fd, line, len);
		if (bytes_written < 0)
			return (0);
	}
	bytes_written = write(fd, "\n", 1);
	if (bytes_written < 0)
			return (0);
	return (1);
}
