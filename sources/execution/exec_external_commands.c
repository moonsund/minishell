#include "minishell.h"
#include "libft.h"

int		execute_external_commands(t_shell *minishell, t_command *cmd);
void	fetch_and_check_bin_path(t_shell *minishell, char *cmd);

char	*build_path_to_check(char *dir, char c, char *cmd)
{
	char	*path_to_check;
	char	*tmp;

	path_to_check = ft_strjoin(dir, "/");
	tmp = ft_strdup(path_to_check);
	free(path_to_check);
	path_to_check = ft_strjoin(tmp, cmd);
	free(tmp);
	return (path_to_check);
}

void	fetch_and_check_bin_path(t_shell *minishell, char *cmd)
{
	char	*path_var_in_env;
	char	**all_directories_from_path;
	char	*path_to_check;
	int		i;

	i = 0;
	if (ft_strchr(cmd, '/'))			// Command is already entered as binary
	{
		if (access(cmd, X_OK) == 0)		// Success = Command is executable
			return;
		perror("error");
	}
	path_var_in_env = getenv("PATH");
	all_directories_from_path = ft_split(path_var_in_env, ':');			// Should I select only the ones containing 'bin' ?
	while (all_directories_from_path[i])
	{
		path_to_check = build_path_to_check(all_directories_from_path[i], '/', cmd);
		if (access(path_to_check, X_OK) == 0)
		{
			free(cmd);							// Check w/ Leon if cmds were malloc'ed during parsing. If not, don't free
			cmd = ft_strdup(path_to_check);		// To keep the same address (easier to debug)
			free(path_to_check);
			free(all_directories_from_path);
			return;
		}
		i++;
	}
	// Bin command not found = error message dealt with later - Nothing to do here (TBC)
	free(path_to_check);
	free(all_directories_from_path);
}

int		execute_external_commands(t_shell *minishell, t_command *cmd)
{
	char	**envp;
	envp = build_envp(&minishell->env_vars);

	char	**execve_args;

	if (cmd->argv)
	{
		execve_args = cmd->argv;
		fetch_and_check_bin_path(minishell, execve_args[0]);
	}

	execve(execve_args[0], execve_args, envp);
	// if execve fails :
	free (envp);									// How to free envp if execve doesn't fail ? Put in struct ?
	return (0);
}
