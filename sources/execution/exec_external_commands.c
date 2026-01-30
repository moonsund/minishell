#include "minishell.h"
#include "libft.h"

char	*build_path_to_check(char *dir, char c, char *cmd);
char	*fetch_and_check_bin_path(t_shell *minishell, char *cmd);
int		execute_external_commands(t_shell *minishell, t_command *cmd);

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

char	*fetch_and_check_bin_path(t_shell *minishell, char *cmd)
{
	char	*path_var_in_env;
	char	**all_directories_in_path_var;
	char	*path_to_check;
	int		i;

	i = 0;
	if (ft_strchr(cmd, '/'))			// Command is already entered as binary
	{
		if (access(cmd, X_OK) == 0)		// Success = Command is executable
			return (cmd);
		perror("error");
		return (NULL);
	}
	path_var_in_env = getenv("PATH");
	all_directories_in_path_var = ft_split(path_var_in_env, ':');
	while (all_directories_in_path_var[i])
	{
		path_to_check = build_path_to_check(all_directories_in_path_var[i], '/', cmd);	// Malloc
		if (access(path_to_check, X_OK) == 0)
		{
			free(all_directories_in_path_var);
			return (path_to_check);
		}
		free(path_to_check);
		i++;
	}
	// Bin command not found = error message dealt with later - Nothing to do here (TBC)
	free(all_directories_in_path_var);
	return (NULL);
}

int		execute_external_commands(t_shell *minishell, t_command *cmd)
{
	char	**envp;
	char	**execve_args;
	char	*updated_path;

	envp = build_envp(&minishell->env_vars);
	if (cmd->argv)
	{
		execve_args = cmd->argv;
		updated_path = fetch_and_check_bin_path(minishell, execve_args[0]);
	}
	execve(updated_path, execve_args, envp);
	// if execve fails :
	free (envp);
	return (0);
}
