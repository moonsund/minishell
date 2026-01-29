#include "minishell.h"

int			execute_pipeline(t_shell *shell);
int 		run_builtin_without_output_in_parent(t_shell *shell, t_command *cmd);
int			run_any_builtin_in_child(t_shell *shell, t_command *cmd);
int			exec_pipeline_forking(t_shell *shell, const t_pipeline *pl);
static void	apply_redirs_or_die(const t_command *cmd);
static int	open_redir_file(const t_redir *redir);
static void	close_if_valid(int fd);
static int	wait_all_and_get_last(pid_t *pids, size_t count);
int is_parent_builtin(const char *cmd);
int is_builtin(const char *cmd);

/*
Redirections - Test commands :
wc -l < infile
sort < infile
grep ok << end
pwd > outfile
ls >> outfile
export FRUIT=apple > new_outile
> outfile		(this command invites user to write lines, then puts them in the outfile, heredoc-style)
>> outfile		(this command invites user to write lines, then adds them to the outfile, heredoc-style)
cat w y z > new

Pipes - Test commands :
ls | grep sources | wc
ls -la | grep git | wc -l
cat z | sort | head -5
ls | exit | wc
pwd | grep z | wc -m
echo London | cat -e > y

*/

int execute_pipeline(t_shell *shell)
{
	t_pipeline *pl;
	t_command	*cmd;

	pl = shell->pipeline;

	if (pl->count == 0 || !pl->cmds)
		return (ES_GENERAL);

	cmd = &pl->cmds[0];

	// cd/export/unset/exit with and w/o redirections = all commands that don't print anything but modify the shell
	// if (pl->count == 1 && cmd->argv && cmd->argv[0] && is_builtin_test(cmd->argv[0], true))
	if (pl->count == 1 && cmd->argv && cmd->argv[0] && is_parent_builtin(cmd->argv[0]))
		return (run_builtin_without_output_in_parent(shell, cmd));

	// Command line starting with a redirection, but no pipe, no cmd and no other redirection, e.g. '> outfile'
	if((shell->pipeline->count == 1) && (!shell->pipeline->cmds->argv) &&
		shell->pipeline->cmds->redirs &&
			(shell->pipeline->cmds->redirs->type == R_OUT ||
				shell->pipeline->cmds->redirs->type == R_APPEND))
	{
		add_user_input_to_fd(shell);
		return (0);
	}

	// only redirections, builtins and external commands both with and w/o redirections
	return (exec_pipeline_forking(shell, pl));
}

int is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);

	return (
		ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0
	);
}

int is_parent_builtin(const char *cmd)
{
	if (!cmd)
		return (0);

	return (
		ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "exit") == 0
	);
}


// examples: cd /tmp > out.txt or unset PATH
// save backup of stdin/stdout (dup)				-- No need anymore, as nothing is happening in the fd
// apply redirections (dup2 to the required fds)	-- No need anymore, as nothing is happening in the fd
// execute the builtin
// restore stdin/stdout (dup2 back)					-- No need anymore, as nothing is happening in the fd
// close backup fds									-- No need anymore, I only need to close the new fd
int run_builtin_without_output_in_parent(t_shell *shell, t_command *cmd)
{
	int	new_fd;

	new_fd = -1;

	if (cmd->redirs)		// FD opened and closed right after because these commands don't print anything
	{
		if (cmd->redirs->type == R_IN)								// Bash : Do nothing if file exists / Error if file doesn't exist - Error not handled in Minishell
			new_fd = open_fd(cmd->redirs->target, true, false);
		else if (cmd->redirs->type == R_OUT)						// Bash : Erase content if file exists / Create file if doesn't exist
			new_fd = open_fd(cmd->redirs->target, false, true);
		else if (cmd->redirs->type == R_APPEND)						// Bash : Do nothing if file exists / Create file if doesn't exist
			new_fd = open_fd(cmd->redirs->target, true, false);
		close(new_fd);
	}

	if(ft_strcmp(cmd->argv[0], "cd") == 0)
		execute_cd(cmd);
	else if(ft_strcmp(cmd->argv[0], "export") == 0)
		execute_export(shell);
	else if(ft_strcmp(cmd->argv[0], "unset") == 0)
		execute_unset(shell);
	else if(ft_strcmp(cmd->argv[0], "exit") == 0)
		execute_exit(shell);
	return (0);
}

int run_any_builtin_in_child(t_shell *shell, t_command *cmd)
{
	// if (is_builtin_test(cmd->argv[0], true))
	if (is_parent_builtin(cmd->argv[0]))
	{
		if (shell->pipeline->count > 1)	// Builtin without output : cd / export / unset BUT with pipes involved : 'cd | ls' : command ignored, jump to next
			return (0);
		else							// One command only = Normal expected exec
			execute_built_in_commands(shell);
	}
	else										// Builtin with output : process to execution (after FD update TBC ? - if applicable)
	{
		execute_built_in_commands(shell);		// Only builtins w/ ouputs, because the other ones have been filtered out at the start of this function
	}
	return (0);
}

// only redirections, builtins and external commands both with and w/o redirections
int	exec_pipeline_forking(t_shell *shell, const t_pipeline *pl)
{
	size_t	i;
	int		prev_read;
	int		pipefds[2];
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
	while (pl->count > i)  // true while we have commands for the execution
	{
		pipefds[0] = -1; // read end
		pipefds[1] = -1;	// write end
		if (i + 1 < pl->count) // checks if we need a pipe for the current command
		{
			if (pipe(pipefds) < 0)
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
			close_if_valid(pipefds[0]);
			close_if_valid(pipefds[1]);
			close_if_valid(prev_read);
			free(pids);
			return (1);
		}

		if (pid == 0) // child
		{
			if (prev_read != -1) // if not the 1st pipe
			{
				if (dup2(prev_read, STDIN_FILENO) < 0) // dup2(old, new)
				{
					perror("dup2 stdin");
					exit(1);
				}
			}
			// if not the last command
			if (pipefds[1] != -1)
			{
				if (dup2(pipefds[1], STDOUT_FILENO) < 0)
				{
					perror("dup2 stdout");
					exit(1);
				}
			}

			close_if_valid(prev_read);
			close_if_valid(pipefds[0]);
			close_if_valid(pipefds[1]);

			// apply redirs
			apply_redirs_or_die(&pl->cmds[i]);

			// command line with only redirections, e.g. "< in > out"
			if (!pl->cmds[i].argv || !pl->cmds[i].argv[0])
				exit(0);

			// in-pipes redirections without command 'ls | > y' - Nothing to do
			// if ((!pl->cmds[i].argv) && (pl->cmds[i].redirs))
			// 	exit (0);

			// if (is_builtin_test(pl->cmds[i].argv[0], false))
			if (is_builtin(pl->cmds[i].argv[0]))
			{
				last_status = run_any_builtin_in_child(shell, &pl->cmds[i]);
				exit (last_status);
			}
			else
			{
				if (!execute_external_commands(shell, &pl->cmds[i]))
				{
					perror("execve");
					if (errno == ENOENT)     // No such file or directory
						exit (127);
					else
						exit (126);  // EACCES, EISDIR, ENOEXEC, etc.
				}
			}
			// NB: the child MUST ALWAYS terminate with exit(status)
		}
		// parent
		pids[i] = pid;
		close_if_valid(prev_read);
		close_if_valid(pipefds[1]);
		prev_read = pipefds[0];

		i++;
	}
	/*
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
