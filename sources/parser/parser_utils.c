/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:23 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 21:43:40 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		free_cmd(t_command *cmd);
static void	free_cmd_argv(t_command *cmd);
static void	free_cmd_redirs(t_command *cmd);
static bool	is_heredoc_file(const char *filename);

void	free_cmd(t_command *cmd)
{
	if (!cmd)
		return ;
	free_cmd_argv(cmd);
	free_cmd_redirs(cmd);
	init_command(cmd);
}

static void	free_cmd_argv(t_command *cmd)
{
	size_t	i;

	if (!cmd || !cmd->argv)
		return ;
	i = 0;
	while (cmd->argv[i])
	{
		free(cmd->argv[i]);
		i++;
	}
	free(cmd->argv);
	cmd->argv = NULL;
}

static void	free_cmd_redirs(t_command *cmd)
{
	t_redir	*redir;
	t_redir	*next;

	if (!cmd)
		return ;
	redir = cmd->redirs;
	while (redir)
	{
		next = redir->next;
		if (redir->target)
		{
			if (is_heredoc_file(redir->target))
				unlink(redir->target);
			free(redir->target);
		}
		free(redir);
		redir = next;
	}
	cmd->redirs = NULL;
}

static bool	is_heredoc_file(const char *filename)
{
	if (!filename)
		return (false);
	return (ft_strncmp(filename, ".heredoc_", 9) == 0);
}
