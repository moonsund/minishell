/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:37 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 13:31:54 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status			process_heredoc(t_pipeline *pl,
							t_env_var_list *env_vars,
							t_exit_status last_status);
static t_exit_status	process_cmd_heredoc(t_command *cmd, t_hd_ctx *ctx);
static t_exit_status	get_heredoc(t_redir *redir, t_hd_ctx *ctx);
static t_exit_status	hd_write_loop(int fd, t_redir *r, t_hd_ctx *ctx);
static char				*generate_heredoc_filename(size_t heredoc_index);

t_exit_status	process_heredoc(t_pipeline *pl, t_env_var_list *env_vars,
		t_exit_status last_status)
{
	t_hd_ctx		ctx;
	size_t			i;
	t_exit_status	st;

	if (!pl || !env_vars)
		return (ES_GENERAL);
	ctx.env = env_vars;
	ctx.last_status = last_status;
	ctx.index = 0;
	i = 0;
	while (i < pl->count)
	{
		st = process_cmd_heredoc(&pl->cmds[i], &ctx);
		if (st != ES_SUCCESS)
			return (st);
		i++;
	}
	return (ES_SUCCESS);
}

static t_exit_status	process_cmd_heredoc(t_command *cmd, t_hd_ctx *ctx)
{
	t_redir			*r;
	t_exit_status	st;

	if (!cmd || !ctx)
		return (ES_GENERAL);
	r = cmd->redirs;
	while (r)
	{
		if (r->type == R_HEREDOC)
		{
			st = get_heredoc(r, ctx);
			if (st != ES_SUCCESS)
				return (st);
		}
		r = r->next;
	}
	return (ES_SUCCESS);
}

static t_exit_status	get_heredoc(t_redir *redir, t_hd_ctx *ctx)
{
	char			*name;
	int				fd;
	t_exit_status	st;

	name = generate_heredoc_filename(ctx->index++);
	if (!name)
		return (ES_GENERAL);
	fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
		return (hd_abort(-1, name, ES_GENERAL));
	setup_signals_heredoc();
	st = hd_write_loop(fd, redir, ctx);
	if (st != ES_SUCCESS)
		return (hd_abort(fd, name, st));
	close(fd);
	setup_signals();
	redir_replace_with_infile(redir, name);
	return (ES_SUCCESS);
}

static t_exit_status	hd_write_loop(int fd, t_redir *r, t_hd_ctx *ctx)
{
	char	*line;

	while (true)
	{
		line = readline("heredoc> ");
		if (g_sigint)
			return (free(line), ES_SIGINT);
		if (!line)
			return (ES_SUCCESS);
		if (r->target && ft_strcmp(line, r->target) == 0)
			return (free(line), ES_SUCCESS);
		if (!is_empty(line))
			add_history(line);
		if (r->expand && !expand_heredoc(&line, ctx->env, ctx->last_status))
			return (free(line), ES_GENERAL);
		if (!write_line_in_fd(fd, line))
			return (free(line), ES_GENERAL);
		free(line);
	}
}

static char	*generate_heredoc_filename(size_t heredoc_index)
{
	char	*idx;
	char	*name;

	idx = ft_itoa((int)heredoc_index);
	if (!idx)
		return (NULL);
	name = ft_strjoin(".heredoc_", idx);
	free(idx);
	return (name);
}
