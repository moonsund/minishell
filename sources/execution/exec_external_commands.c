#include "minishell.h"
#include "libft.h"

int		execute_external_commands(t_shell *minishell, t_command *cmd);

int		execute_external_commands(t_shell *minishell, t_command *cmd)
{
	char	**envp;
	envp = build_envp(&minishell->env_vars);

	char	**execve_args;

	if (cmd->argv)
	{
		execve_args = cmd->argv;
		replace_cmd_by_binary_path(execve_args[0]);
	}

	execve(execve_args[0], execve_args, envp);
	free (envp);									// How to free if execve doesn't fail ? Put in struct ?
	return (0);
}
