#include "minishell.h"
#include "libft.h"

int 	is_builtin(const char *cmd);
int		is_parent_builtin(const char *cmd);
int 	run_builtin_without_output_in_parent(t_shell *shell, t_command *cmd);
int		run_any_builtin_in_child(t_shell *shell, t_command *cmd);
void	execute_built_in_commands(t_shell *minishell);

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

// We're in a child process.
// If there are pipes, commands without output have been ignored, the others will execute normally with the correct FDs (if applicable)
void	execute_built_in_commands(t_shell *minishell)
{
	char	*current_working_directory;
	current_working_directory = fetch_current_working_directory();

	if(ft_strcmp(minishell->pipeline->cmds->argv[0], "echo") == 0)
		execute_echo(minishell->pipeline->cmds);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "cd") == 0)
		execute_cd(minishell->pipeline->cmds);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "pwd") == 0)
		execute_pwd(current_working_directory);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "export") == 0)
		execute_export(minishell);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "unset") == 0)
		execute_unset(minishell);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "env") == 0)
		execute_env(minishell);
	else if(ft_strcmp(minishell->pipeline->cmds->argv[0], "exit") == 0)
		execute_exit(minishell);
	free(current_working_directory);
}
