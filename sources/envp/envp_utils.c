/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:16:01 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 10:16:02 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_envp_partial(char **envp, size_t used);

void free_envp_partial(char **envp, size_t used)
{
	size_t i;

	if (!envp)
		return;

	i = 0;
	while (i < used)
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}
