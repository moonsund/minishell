/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:16:01 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 12:42:22 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_envp_partial(char **envp, size_t used);

void	free_envp_partial(char **envp, size_t used)
{
	size_t	i;

	if (!envp)
		return ;
	i = 0;
	while (i < used)
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}
