/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:39 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 18:23:03 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_empty(const char *str);
bool	is_space(unsigned char c);
bool	is_operator(unsigned char c);
void	append_token(t_token_list *list, t_token *token);

bool	is_empty(const char *str)
{
	if (!str || !*str)
		return (true);
	while (*str)
	{
		if (!is_space((unsigned char)*str))
			return (false);
		str++;
	}
	return (true);
}

bool	is_space(unsigned char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

bool	is_operator(unsigned char c)
{
	return (c == '<' || c == '>' || c == '|');
}

void	append_token(t_token_list *list, t_token *token)
{
	if (!token)
		return ;
	if (list->head == NULL)
	{
		list->tail = token;
		list->head = token;
	}
	else
	{
		list->tail->next = token;
		list->tail = token;
	}
	list->count++;
}
