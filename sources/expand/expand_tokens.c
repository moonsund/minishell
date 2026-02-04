/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:54:59 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 16:24:00 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exit_status			expand_tokens(t_token_list *tokens,
							t_env_var_list *env_vars,
							t_exit_status exit_status);
static t_exit_status	expand_word_token(t_token *token,
							t_env_var_list *env_vars,
							t_exit_status exit_status);
static bool				token_needs_expansion(t_token *token);
static t_exit_status	exp_commit(t_expctx *c);
static t_exit_status	exp_fail(t_expctx *c);

t_exit_status	expand_tokens(t_token_list *tokens, t_env_var_list *env_vars,
		t_exit_status exit_status)
{
	t_token			*prev;
	t_token			*cur;
	t_exit_status	new_exit_status;

	if (!tokens)
		return (ES_GENERAL);
	prev = NULL;
	cur = tokens->head;
	while (cur)
	{
		if (cur->type == TOK_WORD)
		{
			if (!prev || prev->type != TOK_HEREDOC)
			{
				new_exit_status = expand_word_token(cur, env_vars, exit_status);
				if (new_exit_status != ES_SUCCESS)
					return (new_exit_status);
			}
		}
		prev = cur;
		cur = cur->next;
	}
	return (ES_SUCCESS);
}

static t_exit_status	expand_word_token(t_token *token,
		t_env_var_list *env_vars, t_exit_status exit_status)
{
	t_expctx	c;

	if (!token || token->type != TOK_WORD || !token->quotes_map
		|| !token->raw_str)
		return (ES_GENERAL);
	if (!token_needs_expansion(token))
		return (ES_SUCCESS);
	c.tok = token;
	c.env = env_vars;
	c.last = exit_status;
	init_buffer(&c.buf);
	if (exp_loop(&c) != ES_SUCCESS)
		return (exp_fail(&c));
	return (exp_commit(&c));
}

static bool	token_needs_expansion(t_token *token)
{
	size_t		i;
	const char	*str;
	t_qmark		*q_map;

	str = token->raw_str;
	q_map = token->quotes_map;
	i = 0;
	while (i < token->length)
	{
		if (str[i] == '$' && q_map[i] != Q_SQ)
			return (true);
		i++;
	}
	return (false);
}

static t_exit_status	exp_fail(t_expctx *c)
{
	free_buf(&c->buf);
	return (ES_GENERAL);
}

static t_exit_status	exp_commit(t_expctx *c)
{
	free(c->tok->raw_str);
	free(c->tok->quotes_map);
	c->tok->quotes_map = c->buf.quotes_map;
	c->tok->raw_str = c->buf.characters;
	c->tok->length = c->buf.used_length - 1;
	return (ES_SUCCESS);
}
