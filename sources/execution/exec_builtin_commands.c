#include "minishell.h"
#include "libft.h"

char	*fetch_current_working_directory(void);
void	execute_echo(t_command *cmds);
bool	is_line_return(char **cmd, int *i);
void	execute_cd(t_command *cmds);
void	execute_pwd(char *current_working_directory);

char	*fetch_current_working_directory(void)
{
	char	*current_working_directory;

	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);						// Ⓜ️
	if (!current_working_directory)
	{
		perror("");
		return(NULL);
	}
	getcwd(current_working_directory, PATH_MAX);
	return (current_working_directory);
}

// Subject : "echo with option -n"
void	execute_echo(t_command *cmds)
{
	int		i;
	bool	line_return;
	char	*all_argv;
	char	**separate_words;

	line_return = is_line_return(cmds->argv, &i);

	while (cmds->argv[i])
	{
		write(1, cmds->argv[i], ft_strlen(cmds->argv[i]));
		if (cmds->argv[i+1])
		{
			write(1, " ", 1);
		}
		i++;
	}

	if(line_return == true)
		write(1, "\n", 1);
}

bool	is_line_return(char **cmd, int *i)
{
	if (cmd[1] && (ft_strcmp(cmd[1], "-n") == 0))
	{
		*i = 2;
		return (false);
	}
	else
	{
		*i = 1;
		return (true);
	}
}

// Subject : "cd with only a relative or absolute path"
void	execute_cd(t_command *cmds)
{
	if (cmds->argv[1])
	{
		if(chdir(cmds->argv[1]) == -1)
		{
			perror("Error");
		}
	}
}

// Subject : "pwd with no options"
void	execute_pwd(char *current_working_directory)
{
	printf("%s\n", current_working_directory);
}

// Subject : "exit with no options"
void	execute_exit(t_shell *minishell)
{
	// Free memory
	exit (minishell->exit_status);
}
