#include "minishell.h"

static int init_shell(t_shell *shell, char **envp);
static void print_token_list(t_token_list *list);

int main(int argc, char **argv, char **envp)
{
	char *line;
	t_shell	shell;

	shell.exit_status = 1;
	(void)argc;
	(void)argv;

	if (!init_shell(&shell, envp))
		return (EXIT_FAILURE);

	setup_signals();
	
	while(true) // or exit_status
	{
		line = readline("minishell> ");
		if (!line)
		{
			printf("exit\n");
			break;
		}

		if (!is_empty(line))
			add_history(line);
		printf("[readline_debug]: \"%s\"\n", line);

		if (!tokenize_with_qmap(line, &shell.tokens))
		{
			printf("minishell: syntax error: unexpected end of file\n");
			shell.exit_status = 2;
			free(line);
			continue;
		}
		print_token_list(&shell.tokens);	// for debugging, to be deleted (note from Sophie : super useful ! Please don't delete yet ^^')

		expand_tokens(&shell.tokens, &shell.env_vars);

		print_token_list(&shell.tokens); // for debugging, to be deleted

		// if (!build_pipeline_from_tokens(&shell))
		// {
		// 	free_tokens(&shell.tokens);
		// 	free(line);
		// 	continue;
		// }

		// print_pipe_line(&shell.pipeline);	// for debugging, to be deleted

		// check_command_type _and_execute(shell);	// Exec testing starts here
		shell.pipeline = NULL;
		free_tokens(&shell.tokens);
		free(line);
	}

	return (EXIT_SUCCESS);
}

static int init_shell(t_shell *shell, char **envp)
{
	shell->tokens.head = NULL;
	shell->tokens.count = 0;
	shell->pipeline = NULL;

	if (!init_env_var_list(&shell->env_vars, envp))
		return (0);
	return (1);
}

static void print_token_list(t_token_list *tokens) // for debugging, to be deleted
{
	static const char *g_token_type_str[] =
	{
		[TOK_WORD] = "WORD",
		[TOK_PIPE] = "PIPE",
		[TOK_REDIR_IN] = "REDIR_IN",
		[TOK_HEREDOC] = "HEREDOC",
		[TOK_REDIR_OUT] = "REDIR_OUT",
		[TOK_APPEND] = "APPEND",
	};

	static const char qmark_char[] =
	{
		[Q_NONE] = 'N',
		[Q_SQ] = 'S',
		[Q_DQ] = 'D',
	};

	const t_token *token = tokens->head;
	size_t idx = 0;
	while (token)
	{
		const char *type_str = g_token_type_str[token->type];
		printf("[%zu] %-4s : \"%s\"", idx, type_str, token->raw_str ? token->raw_str : "");
		if (token->type == TOK_WORD) {
			printf("  qmap: ");
			for (size_t i = 0; i < token->length; ++i)
				putchar(qmark_char[token->quotes_map[i]]);
		}
		putchar('\n');
		token = token->next;
		idx++;
	}
}


// void print_pipe_line(t_pipeline *pipeline)
// {



// }