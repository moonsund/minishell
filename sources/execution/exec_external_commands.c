#include "minishell.h"
#include "libft.h"

int		execute_external_commands(t_shell *minishell, t_command *cmd);
void	fork_and_exec(t_shell *minishell, int *fd_in, int *fd_out, char **execve_args);
void	parent_process_actions(t_shell *minishell, int fork_pid_return, int *fd_in, int *fd_out);
void	child_process_actions(char **execve_args, char **envp, int *fd_in, int *fd_out);

int		execute_external_commands(t_shell *minishell, t_command *cmd)
{
	char	**envp;
	envp = build_envp(&minishell->env_vars);

	t_command	*all_commands = minishell->pipeline->cmds;

	// char		*debug_string;
	// debug_string = ft_calloc(sizeof(char), 101);

	char	**execve_args;

	if (cmd->argv)
	{
		execve_args = cmd->argv;
		replace_cmd_by_binary_path(execve_args[0]);
	}

	execve(execve_args[0], execve_args, envp);
	return (0);
}
