/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:09 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 13:49:23 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	err_print(t_exit_status type, const char *ctx);
void	err_malloc_print(const char *where);

void	err_print(t_exit_status type, const char *where)
{
	if (type == ES_GENERAL)
	{
		write(2, "minishell: ", 11);
		write(2, where, ft_strlen(where));
		write(2, "\n", 1);
	}
	else if (type == ES_INVALID_USAGE)
	{
		write(2, "minishell: syntax_error: ", 25);
		write(2, where, ft_strlen(where));
		write(2, "\n", 1);
	}
	else
	{
		write(2, "minishell: ", 11);
		write(2, where, ft_strlen(where));
		write(2, "\n", 1);
	}
}

void	err_malloc_print(const char *where)
{
	write(2, "minishell: ", 11);
	write(2, where, ft_strlen(where));
	write(2, ": cannot allocate memory\n", 26);
}
