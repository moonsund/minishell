/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:01 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 12:49:44 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status			expand_heredoc(char **line, t_env_var_list *env_vars,
							t_exit_status exit_status);
static t_exit_status	expand_dollar(t_strbuf *strbuf, const char *src,
							size_t *i, t_env_var_list *env_vars,
							t_exit_status exit_status);
static size_t			var_name_end(const char *str, size_t start);
static t_exit_status	append_env_value(t_strbuf *strbuf,
							t_env_var_list *env_vars, const char *s,
							size_t start, size_t end);

t_exit_status	expand_heredoc(char **line, t_env_var_list *env_vars,
		t_exit_status exit_status)
{
	t_strbuf		strbuf;
	const char		*src;
	size_t			i;
	t_exit_status	status;

	if (!line || !*line || !env_vars)
		return (ES_GENERAL);
	status = strbuf_init(&strbuf);
	if (status != ES_SUCCESS)
		return (status);
	src = *line;
	i = 0;
	while (src[i])
	{
		if (src[i] == '$')
			status = expand_dollar(&strbuf, src, &i, env_vars, exit_status);
		else
		{
			status = strbuf_append_char(&strbuf, src[i]);
			i++;
		}
		if (status != ES_SUCCESS)
		{
			strbuf_free(&strbuf);
			return (status);
		}
	}
	free(*line);
	*line = strbuf.buf;
	return (ES_SUCCESS);
}

static t_exit_status	expand_dollar(t_strbuf *strbuf, const char *src,
		size_t *i, t_env_var_list *env_vars, t_exit_status exit_status)
{
	size_t			start;
	size_t			end;
	t_exit_status	status;

	if (!src[*i + 1])
	{
		status = strbuf_append_char(strbuf, '$');
		if (status != ES_SUCCESS)
			return (status);
		*i += 1;
		return (ES_SUCCESS);
	}
	if (src[*i + 1] == '?')
	{
		status = strbuf_append_str(strbuf, get_last_status_string(exit_status));
		if (status != ES_SUCCESS)
			return (status);
		*i += 2;
		return (ES_SUCCESS);
	}
	start = *i + 1;
	end = var_name_end(src, start);
	if (end == start)
	{
		status = strbuf_append_char(strbuf, '$');
		if (status != ES_SUCCESS)
			return (status);
		*i += 1;
		return (ES_SUCCESS);
	}
	status = append_env_value(strbuf, env_vars, src, start, end);
	if (status != ES_SUCCESS)
		return (status);
	*i = end;
	return (ES_SUCCESS);
}

static size_t	var_name_end(const char *str, size_t start)
{
	size_t	j;

	j = start;
	while (str[j] && (ft_isalnum((unsigned char)str[j]) || str[j] == '_'))
		j++;
	return (j);
}

static t_exit_status	append_env_value(t_strbuf *strbuf,
		t_env_var_list *env_vars, const char *s, size_t start, size_t end)
{
	char	*name;
	char	*val;

	name = (char *)malloc((end - start) + 1);
	if (!name)
		return (ES_GENERAL);
	ft_memcpy(name, s + start, end - start);
	name[end - start] = '\0';
	val = get_var_value(env_vars, name);
	free(name);
	if (val && val[0] != '\0')
		return (strbuf_append_str(strbuf, val));
	return (ES_SUCCESS);
}
