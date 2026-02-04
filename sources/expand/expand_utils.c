/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 13:41:51 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 16:25:08 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status	exp_loop(t_expctx *c);
t_exit_status	exp_append_char(t_expctx *c, char ch);
t_exit_status	exp_dollar(t_expctx *c, size_t *i);
t_exit_status	exp_append_status(t_expctx *c, size_t *i);
int				append_str(t_buf *buf, char *str, t_qmark quote_mark);

t_exit_status	exp_loop(t_expctx *c)
{
	size_t	i;

	i = 0;
	while (i < c->tok->length)
	{
		if (c->tok->raw_str[i] == '$')
		{
			if (exp_dollar(c, &i) != ES_SUCCESS)
				return (ES_GENERAL);
			continue ;
		}
		if (exp_append_char(c, c->tok->raw_str[i]) != ES_SUCCESS)
			return (ES_GENERAL);
		i++;
	}
	if (exp_append_char(c, '\0') != ES_SUCCESS)
		return (ES_GENERAL);
	return (ES_SUCCESS);
}

t_exit_status	exp_dollar(t_expctx *c, size_t *i)
{
	size_t	start;
	size_t	end;

	start = *i + 1;
	if (start >= c->tok->length)
		return (exp_append_char(c, c->tok->raw_str[(*i)++]));
	if (c->tok->raw_str[start] == '?')
		return (exp_append_status(c, i));
	end = var_end(c->tok->raw_str, c->tok->length, start);
	if (end == start)
		return (exp_append_char(c, c->tok->raw_str[(*i)++]));
	return (exp_append_var(c, start, end, i));
}

t_exit_status	exp_append_status(t_expctx *c, size_t *i)
{
	char	*val;

	val = get_last_status_string(c->last);
	if (!val || !append_str(&c->buf, val, Q_NONE))
	{
		free(val);
		return (ES_GENERAL);
	}
	free(val);
	*i += 2;
	return (ES_SUCCESS);
}

t_exit_status	exp_append_char(t_expctx *c, char ch)
{
	if (!append_char(ch, &c->buf, Q_NONE))
		return (ES_GENERAL);
	return (ES_SUCCESS);
}

int	append_str(t_buf *buf, char *str, t_qmark quote_mark)
{
	size_t	i;
	size_t	str_length;

	str_length = ft_strlen(str);
	if (!boost_buf(buf, buf->used_length + str_length))
		return (0);
	i = 0;
	while (i < str_length)
	{
		buf->characters[buf->used_length] = str[i];
		buf->quotes_map[buf->used_length] = quote_mark;
		buf->used_length++;
		i++;
	}
	return (1);
}
