/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorlov <lorlov@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:14:09 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 10:14:10 by lorlov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status strbuf_init(t_strbuf *strbuf);
void strbuf_free(t_strbuf *strbuf);
t_exit_status strbuf_reserve(t_strbuf *strbuf, size_t extra);
t_exit_status strbuf_append_char(t_strbuf *strbuf, char c);
t_exit_status strbuf_append_str(t_strbuf *strbuf, const char *s);

t_exit_status strbuf_init(t_strbuf *strbuf)
{
	strbuf->buf = (char *)malloc(1);
	if (!strbuf->buf)
		return (ES_GENERAL);
	strbuf->buf[0] = '\0';
	strbuf->len = 0;
	strbuf->cap = 1;
	return (ES_SUCCESS);
}

void strbuf_free(t_strbuf *strbuf)
{
	if (strbuf && strbuf->buf)
		free(strbuf->buf);
	if (strbuf)
	{
		strbuf->buf = NULL;
		strbuf->len = 0;
		strbuf->cap = 0;
	}
}

t_exit_status strbuf_reserve(t_strbuf *strbuf, size_t extra)
{
	char    *tmp;
	size_t  need;
	size_t  new_cap;

	need = strbuf->len + extra + 1;
	if (need <= strbuf->cap)
		return (ES_SUCCESS);

	new_cap = strbuf->cap;
	while (new_cap < need)
		new_cap *= 2;

	tmp = (char *)malloc(new_cap);
	if (!tmp)
		return (ES_GENERAL);

	ft_memcpy(tmp, strbuf->buf, strbuf->len + 1);
	free(strbuf->buf);
	strbuf->buf = tmp;
	strbuf->cap = new_cap;
	return (ES_SUCCESS);
}

t_exit_status strbuf_append_char(t_strbuf *strbuf, char c)
{
	t_exit_status st;

	st = strbuf_reserve(strbuf, 1);
	if (st != ES_SUCCESS)
		return (st);

	strbuf->buf[strbuf->len] = c;
	strbuf->len += 1;
	strbuf->buf[strbuf->len] = '\0';
	return (ES_SUCCESS);
}

t_exit_status strbuf_append_str(t_strbuf *strbuf, const char *s)
{
	size_t        n;
	t_exit_status st;

	if (!s || s[0] == '\0')
		return (ES_SUCCESS);

	n = ft_strlen(s);
	st = strbuf_reserve(strbuf, n);
	if (st != ES_SUCCESS)
		return (st);

	ft_memcpy(strbuf->buf + strbuf->len, s, n);
	strbuf->len += n;
	strbuf->buf[strbuf->len] = '\0';
	return (ES_SUCCESS);
}