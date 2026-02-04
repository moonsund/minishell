/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_cmds_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:08 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/04 14:37:42 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

char		*fetch_current_working_directory(void);
bool		is_line_return(char **cmd, int *i);
bool		is_numeric_string(const char *str);
int			parse_exit_code(const char *str, int *exit_code);
static int	parse_digits(const char *str, int i, long long *result);

char	*fetch_current_working_directory(void)
{
	char	*current_working_directory;

	current_working_directory = ft_calloc(sizeof(char), PATH_MAX);
	if (!current_working_directory)
	{
		perror("calloc: current_working_directory");
		return (NULL);
	}
	getcwd(current_working_directory, PATH_MAX);
	return (current_working_directory);
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

int	parse_exit_code(const char *str, int *exit_code)
{
	long long	result;
	int			sign;
	int			i;

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
	i = parse_digits(str, i, &result);
	if (i == -1)
		return (-1);
	result = sign * result;
	*exit_code = (int)(result % 256);
	if (*exit_code < 0)
		*exit_code += 256;
	return (0);
}

static int	parse_digits(const char *str, int i, long long *result)
{
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (*result > (LLONG_MAX - (str[i] - '0')) / 10)
			return (-1);
		*result = *result * 10 + (str[i] - '0');
		i++;
	}
	return (i);
}

bool	is_numeric_string(const char *str)
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
