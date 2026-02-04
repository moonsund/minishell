/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:20 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/04 21:45:25 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		token_has_any_quotes(t_token *token);
int		redir_push_back(t_redir **lst, t_redir *node);
size_t	count_argv(char **argv);
char	**alloc_argv_with_copy(char **old_argv, size_t argc);
int		append_dup_arg(char **new_argv, size_t argc, char *arg);

int	token_has_any_quotes(t_token *token)
{
	size_t	i;

	i = 0;
	if (!token || !token->quotes_map)
		return (0);
	while (i < token->length)
	{
		if (token->quotes_map[i] == Q_SQ || token->quotes_map[i] == Q_DQ)
			return (1);
		i++;
	}
	return (0);
}

int	redir_push_back(t_redir **lst, t_redir *node)
{
	t_redir	*cur;

	if (!node)
		return (0);
	if (!*lst)
	{
		*lst = node;
		return (1);
	}
	cur = *lst;
	while (cur->next)
		cur = cur->next;
	cur->next = node;
	return (1);
}

size_t	count_argv(char **argv)
{
	size_t	argc;

	argc = 0;
	if (!argv)
		return (0);
	while (argv[argc])
		argc++;
	return (argc);
}

char	**alloc_argv_with_copy(char **old_argv, size_t argc)
{
	char	**new_argv;
	size_t	i;

	new_argv = (char **)malloc(sizeof(*new_argv) * (argc + 2));
	if (!new_argv)
		return (NULL);
	i = 0;
	while (i < argc)
	{
		new_argv[i] = old_argv[i];
		i++;
	}
	return (new_argv);
}

int	append_dup_arg(char **new_argv, size_t argc, char *arg)
{
	char	*dup;

	dup = ft_strdup(arg);
	if (!dup)
		return (0);
	new_argv[argc] = dup;
	new_argv[argc + 1] = NULL;
	return (1);
}
