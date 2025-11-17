#include "minishell.h"

static void print_token_list(t_token_list *list);

int main(int argc, char **argv, char **envp)
{
	char *line;
	t_shell	shell;

	shell.exit_status = 1;
	(void)argc;
	(void)argv;
	(void)envp;


	setup_signals();
	// init();
	shell.tokens.head = NULL;
	shell.tokens.count = 0;
	shell.pipeline = NULL;
	while(true) // or exit_status
	{
		line = readline("minishell> ");
		if (!line)
		{
			ft_printf("exit\n");
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
		print_token_list(&shell.tokens);		// for debugging, to be deleted (note from Sophie : super useful ! Please don't delete yet ^^')

		// shell.pipeline = parse_command(shell.tokens);
		check_command_type_and_execute(shell);												// Exec testing starts here
		free_tokens(&shell.tokens);
	}

	return (EXIT_SUCCESS);
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
