#include "minishell.h"

static int	open_redir_file(const t_redir *redir);
static void	apply_redirs_or_die(const t_command *cmd);
static int	wait_all_and_get_last(pid_t *pids, size_t count);
static void	close_if_valid(int fd);

int	process_pipeline(const t_pipeline *pl, char **envp)
{
	size_t	i;
	int		prev_read;
	int		pipefd[2];
	pid_t	*pids;
	pid_t	pid;
	int		last_status;

	if (!pl || pl->count == 0)
		return (0);

	pids = (pid_t *)malloc(sizeof(pid_t) * pl->count);
	if (!pids)
		return (1);

	i = 0;
	prev_read = -1;
	while (i < pl->count)  // true while we have commands for the execution
	{
		pipefd[0] = -1;
		pipefd[1] = -1;
		if (i + 1 < pl->count) // checks if we need a pipe for the current command
		{
			if (pipe(pipefd) < 0)
			{
				perror("pipe");
				free(pids);
				return (1);
			}
		}

		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			close_if_valid(pipefd[0]);
			close_if_valid(pipefd[1]);
			close_if_valid(prev_read);
			free(pids);
			return (1);
		}

		if (pid == 0) // child
		{	
			if (prev_read != -1) // if not the 1st pipe
			{
				if (dup2(prev_read, STDIN_FILENO) < 0)
				{
					perror("dup2 stdin");
					exit(1);
				}
			}
			// if not the last command
			if (pipefd[1] != -1)
			{
				if (dup2(pipefd[1], STDOUT_FILENO) < 0)
				{
					perror("dup2 stdout");
					exit(1);
				}
			}

			close_if_valid(prev_read);
			close_if_valid(pipefd[0]);
			close_if_valid(pipefd[1]);

			// apply redirs
			apply_redirs_or_die(&pl->cmds[i]);

			// command line with only redirections, e.g. "< in > out"
			if (!pl->cmds[i].argv || !pl->cmds[i].argv[0])
				exit(0);

			// void execute(); NB 
			// Determine whether it is a builtin;
			// get the PATH;
			// check the rights;


			perror(pl->cmds[i].argv[0]);
			exit(127);
		}

		// parent
		pids[i] = pid;
		close_if_valid(prev_read);
		close_if_valid(pipefd[1]);
		prev_read = pipefd[0];

		i++;
	}

	close_if_valid(prev_read);
	last_status = wait_all_and_get_last(pids, pl->count);
	free(pids);
	return (last_status);
}

static void	apply_redirs_or_die(const t_command *cmd)
{
	t_redir	*redir;
	int		fd;

	redir = cmd->redirs;
	while (redir)
	{
		fd = open_redir_file(redir);
		if (fd < 0)
		{
			perror(redir->target);
			exit(1);
		}
		if (dup2(fd, redir->fd) < 0)
		{
			perror("dup2");
			close(fd);
			exit(1);
		}
		close(fd);
		redir = redir->next;
	}
}

static int	open_redir_file(const t_redir *redir)
{
	int	fd;

	fd = -1;
	if (redir->type == R_IN)
		fd = open(redir->target, O_RDONLY);
	else if (redir->type == R_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == R_APPEND)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redir->type == R_HEREDOC)
		fd = open(redir->target, O_RDONLY);
	return (fd);
}

static void	close_if_valid(int fd)
{
	if (fd >= 0)
		close(fd);
}

static int	wait_all_and_get_last(pid_t *pids, size_t count)
{
	size_t	i;
	int		status;
	int		last_status;

	i = 0;
	last_status = 0;
	while (i < count)
	{
		if (waitpid(pids[i], &status, 0) > 0)
		{
			if (i + 1 == count)
			{
				if (WIFEXITED(status))
					last_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					last_status = 128 + WTERMSIG(status);
				else
					last_status = 1;
			}
		}
		i++;
	}
	return (last_status);
}
