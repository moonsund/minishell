/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 10:15:04 by lorlov            #+#    #+#             */
/*   Updated: 2026/02/05 15:45:12 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	minishell_loop(t_shell *shell);
char	*handle_input(void);
void	process_input(char *input, t_shell *shell);
int		parse_and_prepare(t_shell *shell, char *input);
int		execute_prepared(t_shell *shell);

void	minishell_loop(t_shell *shell)
{
	char	*input;

	while (!shell->should_exit)
	{
		input = handle_input();
		if (!input)
		{
			printf("exit\n");
			shell->should_exit = true;
			break ;
		}
		if (g_sigint)
		{
			g_sigint = 0;
			shell->exit_status = ES_SIGINT;
			free(input);
			reset_iteration(shell);
			continue ;
		}
		process_input(input, shell);
		free(input);
		reset_iteration(shell);
	}
}

char	*handle_input(void)
{
	char	*line;

	line = readline("minishell> ");
	if (!line)
		return (NULL);
	if (line && *line)
		add_history(line);
	return (line);
}

void	process_input(char *input, t_shell *shell)
{
	t_exit_status	status;

	status = (t_exit_status)parse_and_prepare(shell, input);
	if (status == ES_SUCCESS)
		status = (t_exit_status)execute_prepared(shell);
	shell->exit_status = status;
}

int	parse_and_prepare(t_shell *shell, char *input)
{
	int	status;

	if (is_empty(input))
	{
		status = ES_SUCCESS;
		return (status);
	}
	status = tokenize_with_qmap(input, &shell->tokens);
	if (status != ES_SUCCESS)
		return (status);
	status = expand_tokens(&shell->tokens, &shell->env_vars,
			shell->exit_status);
	if (status != ES_SUCCESS)
		return (status);
	status = build_pipeline_from_tokens(shell);
	if (status != ES_SUCCESS)
		return (status);
	status = process_heredoc(shell->pl, &shell->env_vars,
			shell->exit_status);
	if (status != ES_SUCCESS)
		return (status);
	return (ES_SUCCESS);
}

int	execute_prepared(t_shell *shell)
{
	return (execute_pipeline(shell));
}
