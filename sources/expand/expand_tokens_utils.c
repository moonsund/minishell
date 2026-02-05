/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:46:52 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 15:46:48 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char			*get_last_status_string(t_exit_status exit_status);
t_exit_status	exp_append_var(t_expctx *c, size_t start, size_t end,
					size_t *i);
size_t			var_end(const char *s, size_t len, size_t start);
char			*dup_var_name(const char *s, size_t start, size_t end);

char	*get_last_status_string(t_exit_status exit_st)
{
	return (ft_itoa(exit_st));
}

size_t	var_end(const char *s, size_t len, size_t start)
{
	size_t	j;

	j = start;
	while (j < len && (ft_isalnum((unsigned char)s[j]) || s[j] == '_'))
		j++;
	return (j);
}

char	*dup_var_name(const char *s, size_t start, size_t end)
{
	size_t	len;
	char	*name;

	len = end - start;
	name = (char *)malloc(len + 1);
	if (!name)
		return (NULL);
	ft_memcpy(name, s + start, len);
	name[len] = '\0';
	return (name);
}

t_exit_status	exp_append_var(t_expctx *c, size_t start, size_t end, size_t *i)
{
	char	*name;
	char	*val;

	name = dup_var_name(c->tok->raw_str, start, end);
	if (!name)
		return (ES_GENERAL);
	val = get_var_value(c->env, name);
	free(name);
	if (val && val[0] != '\0')
	{
		if (!append_str(&c->buf, val, Q_NONE))
			return (ES_GENERAL);
	}
	*i = end;
	return (ES_SUCCESS);
}
