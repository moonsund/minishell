#include "minishell.h"

t_exit_status		process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars, t_exit_status exit_status);
static t_exit_status get_heredoc(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, size_t *heredoc_index);
static t_exit_status heredoc_loop(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, int fd, char *filename);

t_exit_status process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars, t_exit_status last_status)
{
	size_t  i;
	t_redir *redir;
	size_t  heredoc_index;
	t_exit_status status;

	if (!pipeline || !env_vars)
		return (ES_GENERAL);

	heredoc_index = 0;
	i = 0;
	while (i < pipeline->count)
	{
		redir = pipeline->cmds[i].redirs;
		while (redir)
		{
			if (redir->type == R_HEREDOC)
			{
				status = get_heredoc(redir, env_vars, last_status, &heredoc_index);
				if (status != ES_SUCCESS)
					return (status);
			}
			redir = redir->next;
		}
		i++;
	}
	return (ES_SUCCESS);
}

static t_exit_status get_heredoc(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, size_t *heredoc_index)
{
	char        *heredoc_filename;
	int         fd;
	t_exit_status st;

	heredoc_filename = generate_heredoc_filename((*heredoc_index)++);
	if (!heredoc_filename)
		return (ES_GENERAL);

	fd = open(heredoc_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
	{
		free(heredoc_filename);
		return (ES_GENERAL);
	}

	st = heredoc_loop(redir, env_vars, exit_status, fd, heredoc_filename);
	if (st != ES_SUCCESS)
		return (st);

	close(fd);
	redir_replace_with_infile(redir, heredoc_filename);
	return (ES_SUCCESS);
}

static t_exit_status heredoc_loop(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, int fd, char *filename)
{
	char        *line;
	t_exit_status status;

	while (true)
	{
		line = readline("heredoc> ");

		if (g_sigint) /* Ctrl+C */
		{
			g_sigint = 0;
			return (heredoc_cleanup_return(fd, filename, ES_SIGINT));
		}

		if (!line) /* Ctrl+D (EOF) */
			break;

		if (redir->target && ft_strcmp(line, redir->target) == 0)
		{
			free(line);
			break;
		}

		if (redir->expand)
		{
			status = expand_heredoc(&line, env_vars, exit_status);
			if (status != ES_SUCCESS)
			{
				free(line);
				return (heredoc_cleanup_return(fd, filename, status));
			}
		}

		status = write_line_in_fd(fd, line);
		if (status != ES_SUCCESS)
		{
			free(line);
			return (heredoc_cleanup_return(fd, filename, status));
		}

		free(line);
	}

	return (ES_SUCCESS);
}