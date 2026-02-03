/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_commands.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:08 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/03 17:44:30 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

char	*fetch_current_working_directory(void);
int		execute_echo(t_command *cmds);
bool	is_line_return(char **cmd, int *i);
int		execute_cd(t_command *cmds);
int		execute_pwd(char *current_working_directory);
int		execute_exit(t_shell *shell, t_command *cmd);
int	parse_exit_code(const char *str, int *exit_code);
static bool	is_numeric_string(const char *str);

char	*fetch_current_working_directory(void)
{
	char	*current_working_directory;

	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);
	if (!current_working_directory)
	{
		perror("");
		return (NULL);
	}
	getcwd(current_working_directory, PATH_MAX);
	return (current_working_directory);
}

// Subject : "echo with option -n"
int	execute_echo(t_command *cmds)
{
	int		i;

	bool line_return ;
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

bool	is_line_return(char **cmd, int *i)
{
	if (cmd[1] && (cmd[1][0] == '-') && (cmd[1][1] == 'n'))
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

static bool	is_numeric_string(const char *str)
{
	int	i;

	if (!str || !str[0])
		return (false);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i] || !ft_isdigit(str[i]))
		return (false);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (false);
		i++;
	}
	return (true);
}

int	parse_exit_code(const char *str, int *exit_code)
{
	long long	result;
	int			sign;
	int			i;

	if (!str || !str[0])
		return (0);
	if (!is_numeric_string(str))
		return (-1);
	result = 0;
	sign = 1;
	i = 0;
	if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (result > (LLONG_MAX - (str[i] - '0')) / 10)
			return (-1);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	result = sign * result;
	*exit_code = (int)(result % 256);
	if (*exit_code < 0)
		*exit_code = 256 + *exit_code;
	return (0);
}

int	execute_exit(t_shell *shell, t_command *cmd)
{
	// printf("DEBUG\n");
	int	exit_code;

	if (!cmd || !cmd->argv)
	{
		shell->should_terminate = true;
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
			shell->should_terminate = true;
			shell->exit_status = 255;
			return (255);
		}
		shell->exit_status = exit_code;
	}
	shell->should_terminate = true;
	return (shell->exit_status);
}
