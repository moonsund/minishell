/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_commands.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:08 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/03 21:11:44 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

int			execute_echo(t_command *cmds);
int			execute_cd(t_command *cmds);
int			execute_pwd(char *current_working_directory);
int			execute_exit(t_shell *shell, t_command *cmd);

// Subject : "echo with option -n"
int	execute_echo(t_command *cmds)
{
	int		i;
	bool	line_return ;

	line_return = is_line_return(cmds->argv, &i);
	while (cmds->argv[i])
	{
		write(1, cmds->argv[i], ft_strlen(cmds->argv[i]));
		if (cmds->argv[i + 1])
		{
			write(1, " ", 1);
		}
		i++;
	}
	if (line_return == true)
		write(1, "\n", 1);
	return (0);
}

int	execute_cd(t_command *cmds)
{
	if (cmds->argv[1] && cmds->argv[2])
	{
		err_print(1, "too many arguments");
		return (1);
	}
	if (cmds->argv[1])
	{
		if (chdir(cmds->argv[1]) == -1)
		{
			perror("Error");
			return (1);
		}
	}
	return (0);
}

int	execute_pwd(char *current_working_directory)
{
	printf("%s\n", current_working_directory);
	return (0);
}

int	execute_exit(t_shell *shell, t_command *cmd)
{
	int	exit_code;

	if (!cmd || !cmd->argv)
	{
		shell->should_exit = true;
		return (shell->exit_status);
	}
	if (cmd->argv[1] && cmd->argv[2])
	{
		err_print(ES_INVALID_USAGE, "exit: too many arguments");
		return (1);
	}
	if (cmd->argv[1])
	{
		if (parse_exit_code(cmd->argv[1], &exit_code) != 0)
		{
			err_print(ES_INVALID_USAGE, "exit: numeric argument required");
			shell->should_exit = true;
			shell->exit_status = 255;
			return (255);
		}
		shell->exit_status = exit_code;
	}
	shell->should_exit = true;
	return (shell->exit_status);
}
