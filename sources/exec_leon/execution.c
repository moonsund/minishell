#include "minishell.h"

static int	open_redir_file(const t_redir *redir);
static void	apply_redirs_or_die(const t_command *cmd);
static int	wait_all_and_get_last(pid_t *pids, size_t count);
static void	close_if_valid(int fd);

int execute_pipeline(t_shell *shell)
{
	t_pipeline *pl;
	t_command	*cmd;
	
	pl = shell->pipeline;

	if (pl->count == 0 || !pl->cmds)
		return (ES_GENERAL);

	cmd = &pl->cmds[0];

	// cd/export/unset/exit with and w/o redirections
	if (pl->count == 1 && cmd->argv && cmd->argv[0] && is_parent_only_builtin(cmd->argv[0]))
		return (run_built_in_parent(shell, cmd));
	
	// only redirections, builtins and external commands both with and w/o redirections
	return (exec_pipeline_forking(shell, cmd));
}


int is_parent_only_builtin(char *cmd_name)
{
	return (0);
}


int run_built_in_parent(t_shell *shell, t_command *cmd)
{
// examples: cd /tmp > out.txt or unset PATH
// save backup of stdin/stdout (dup)
// apply redirections (dup2 to the required fds)
// execute the builtin
// restore stdin/stdout (dup2 back)
// close backup fds
}

int	exec_pipeline_forking(const t_pipeline *pl, char **envp)
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

			// command line with only redirections, e.g. "< in > out". Couid it be mooved in to execute_pipeline? 
			if (!pl->cmds[i].argv || !pl->cmds[i].argv[0])
				exit(0);

			/* execute();
            if (builtin)
            {
                status = run_builtin(cmd);
                exit(status);
            }
            else
            {
                execve(path, argv, envp);
                perror("execve");
                if (errno == ENOENT)     // No such file or directory
                    exit(127);
                else
                    exit(126);  // EACCES, EISDIR, ENOEXEC, etc.
            }
            NB: the child MUST ALWAYS terminate with exit(status)
            */
        }

        /* parent
        exit status of the WHOLE pipeline = exit status of the LAST command in the pipeline: ls | grep x | wc -l
        When we fork a pipeline:
            we know the pid of each segment
            we know the pid of the last command
            we wait for ALL pids
            but we take the exit status only from last_pid
        Hence, the parent:
            - waits for all
            - returns the status of the last command in the pipeline
        */
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
