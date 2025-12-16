#include "minishell.h"

static int init_shell(t_shell *shell, char **envp);
int build_pipeline_from_tokens(t_shell *shell);
int expand_tokens(t_token_list *tokens, t_env_var_list *env_vars);
static void print_token_list(t_token_list *list);
static void print_pipe_line(t_pipeline *pipeline);

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
		printf("[readline_debug]: \"%s\"\n", line); // for debugging, to be deleted

		if (!tokenize_with_qmap(line, &shell.tokens))
		{
			printf("minishell: syntax error: unexpected end of file\n");
			shell.exit_status = 2;
			free(line);
			continue;
		}
		printf("\n[tokens_list_debug]:\n");
		print_token_list(&shell.tokens);	// for debugging, to be deleted (note from Sophie : super useful ! Please don't delete yet ^^')

		expand_tokens(&shell.tokens, &shell.env_vars);

		printf("\n[expanded_tokens_list_debug]:\n");
		print_token_list(&shell.tokens); // for debugging, to be deleted

		if (!build_pipeline_from_tokens(&shell))
		{
			free_tokens(&shell.tokens);
			free(line);
			shell.exit_status = 258;
			continue;
		}

		printf("\n[pipelines_debug]:\n");
		print_pipe_line(shell.pipeline);	// for debugging, to be deleted

		if (!process_heredoc(shell.pipeline, &shell.env_vars))
		{
			free_tokens(&shell.tokens);
			free(line);
			shell.exit_status = 258;
			continue;
		}

		check_command_type_and_execute(&shell);	// Exec testing starts here
		shell.pipeline = NULL;
		free_tokens(&shell.tokens);
		free(line);
		// free_everything(shell);
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


static void print_pipe_line(t_pipeline *pipeline) // for debugging, to be deleted
{
    size_t i;
    size_t j;

    if (!pipeline || !pipeline->cmds)
        return;

    for (i = 0; i < pipeline->count; i++)
    {
        printf("Command %zu:\n", i);

        if (!pipeline->cmds[i].argv)
            printf("  (no args)\n");
		else
		{
			j = 0;
			while (pipeline->cmds[i].argv[j])
			{
				printf("  arg[%zu]: %s\n", j, pipeline->cmds[i].argv[j]);
				j++;
			}
		}

		if (pipeline->cmds[i].infile)
			printf("infile: %s\n", pipeline->cmds[i].infile);
		if (pipeline->cmds[i].outfile)
			printf("outfile: %s\n", pipeline->cmds[i].outfile);
		printf("append: %i\n", pipeline->cmds[i].append);
		if (pipeline->cmds[i].heredoc_limiter)
			printf("heredoc_limiter: %s\n", pipeline->cmds[i].heredoc_limiter);
		printf("has_heredoc: %i\n", pipeline->cmds[i].has_heredoc);
		printf("heredoc_expand_needed: %i\n", pipeline->cmds[i].heredoc_expand_needed);
    }
}
