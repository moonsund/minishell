/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_close_and_free.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:19:00 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/03 20:33:15 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "minishell.h"

void	close_if_valid(int fd);
void	free_envp(t_env_var_list *list);
void	free_strings_array(char **array);

// Ok to pass the fd rather than a pointer to it ? TBC
// Would it be smart to set to -1  after closing ? TBD
void	close_if_valid(int fd)
{
	if (fd >= 0)
		close(fd);
}

void	free_envp(t_env_var_list *list)
{
	int	i;

	if (!list)
		return ;
	i = list->count;
	while (i > 0)
	{
		unset_var(list, list->head->name);
		i--;
	}
}

void	free_strings_array(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}
