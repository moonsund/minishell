/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:23 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 10:15:24 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_cmd(t_command *cmd);

static bool is_heredoc_file(const char *filename)
{
	if (!filename)
		return (false);
	return (ft_strncmp(filename, ".heredoc_", 9) == 0);
}

void free_cmd(t_command *cmd)
{
	size_t argc;
	t_redir *redir;
	t_redir *next_redir;

	if (cmd->argv)
	{
		argc = 0;
		while (cmd->argv[argc])
		{
			free(cmd->argv[argc]);
			argc++;
		}
	}
	free(cmd->argv);
	
	redir = cmd->redirs;
	while (redir)
	{
		next_redir = redir->next;
		if (redir->target)
		{
			if (is_heredoc_file(redir->target))
				unlink(redir->target);
			free(redir->target);
		}
		free(redir);
		redir = next_redir;
	}
	
	init_command(cmd);
}
