#include "/home/schappuy/00_Root/08_Minishell/includes/minishell.h"
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
	int		fd;
	bool	line_return;
	char	**separate_words;

	fd = 1;
	line_return = is_line_return(cmds->argv, &i);
	if (cmds->outfile)
	{
		if (cmds->append == 1)
			fd = open_fd(cmds->outfile, true, false);
		else
			fd = open_fd(cmds->outfile, false, true);
	}
	separate_words = ft_split(cmds->argv[i], ' ');
	i = 0;
	while(separate_words[i])
	{
		write(fd, separate_words[i], ft_strlen(separate_words[i]));
		if(separate_words[i+1])
			write(fd, " ", 1);
		i++;
	}
	if(line_return == true)
		write(fd, "\n", 1);
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
			perror("Error");											// Errno prints the rest of the message
		}
	}
}

// Subject : "pwd with no options"
void	execute_pwd(char *current_working_directory)
{
	printf("%s\n", current_working_directory);
}
