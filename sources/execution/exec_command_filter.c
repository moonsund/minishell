#include "minishell.h"
#include "libft.h"

void	execute_built_in_commands(t_shell *minishell);

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
