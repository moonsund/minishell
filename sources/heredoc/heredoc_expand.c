/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:01 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 12:11:14 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		expand_heredoc(char **line, t_env_var_list *env_vars,
			t_exit_status last_status);
int		hd_exp_dollar(char **dst, const char *src, size_t *i, t_exp_ctx *ctx);
char	*exp_varname(const char *src, size_t start, size_t len);

int	expand_heredoc(char **line, t_env_var_list *env_vars,
		t_exit_status last_status)
{
	t_exp_ctx	ctx;
	char		*dst;
	size_t		i;

	ctx.env = env_vars;
	ctx.last_status = last_status;
	dst = ft_strdup("");
	if (!dst)
		return (0);
	i = 0;
	while ((*line)[i])
	{
		if ((*line)[i] == '$')
		{
			if (!hd_exp_dollar(&dst, *line, &i, &ctx))
				return (free(dst), 0);
			continue ;
		}
		if (!append_charter(&dst, (*line)[i]))
			return (free(dst), 0);
		i++;
	}
	free(*line);
	*line = dst;
	return (1);
}

int	hd_exp_dollar(char **dst, const char *src, size_t *i, t_exp_ctx *ctx)
{
	size_t	start;
	size_t	j;
	char	*name;
	char	*val;

	if (!src[*i + 1])
		return (append_charter(dst, src[(*i)++]));
	if (src[*i + 1] == '?')
		return ((*i += 2), append_string(dst,
				get_last_status_string(ctx->last_status)));
	start = *i + 1;
	j = start;
	while (src[j] && (ft_isalnum((unsigned char)src[j]) || src[j] == '_'))
		j++;
	if (j == start)
		return (append_charter(dst, src[(*i)++]));
	name = exp_varname(src, start, j - start);
	if (!name)
		return (0);
	val = get_var_value(ctx->env, name);
	free(name);
	if (val && val[0] != '\0')
		return (*i = j, append_string(dst, val));
	*i = j;
	return (1);
}

char	*exp_varname(const char *src, size_t start, size_t len)
{
	char	*name;

	name = malloc(len + 1);
	if (!name)
		return (NULL);
	ft_memcpy(name, src + start, len);
	name[len] = '\0';
	return (name);
}
