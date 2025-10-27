#include "minishell.h"

void	execute_commands(t_shell command/*, TBD */);
void	execute_pwd();

void	execute_commands(t_shell command/*, TBD */)
{
	if(ft_memcmp(command.command_array[0], "pwd", 3) == 0)
	{
		execute_pwd();
	}
	//  WIP in develop/feature_sophie branch
}

void	execute_pwd()
{
	char	*getcwd_return;
	char	*buf;
	buf = malloc(sizeof(char) * 50);
	size_t	z = 50;

	getcwd_return = getcwd(buf, z);
	printf("%s\n", getcwd_return);
	free(buf);
}
