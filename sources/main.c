#include "minishell.h"

static void print_token_list(t_token_list *list);
static void print_pipe_line(t_pipeline *pipeline);

int main(int argc, char **argv, char **envp)
{
	char *line;
	t_exit_status exit_status;
	t_shell	shell;

	(void)argc;
	(void)argv;

	if (!init_shell(&shell, envp))
	{
		err_print(ES_GENERAL, "init_shell");
		return (EXIT_FAILURE);
	}
	setup_signals();

	while(true)
	{
		line = readline("minishell> ");

		if (!line || ft_strcmp(line, "exit") == 0) // Ctrl+D (EOF) or exit command
		{
			printf("exit\n");
			break;
		}

		if (g_sigint) // Ctrl+C
		{
			g_sigint = 0;
			shell.exit_status = ES_SIGINT;
			free(line);
			// rl_on_new_line();
			// rl_replace_line("", 0);
			// rl_redisplay();
			continue;
		}

		if (is_empty(line) || ft_strcmp(line, "") == 0)
		{
			free(line);
			continue;
		}
		add_history(line);


		exit_status = tokenize_with_qmap(line, &shell.tokens);
		if (exit_status != ES_SUCCESS)
		{
			shell.exit_status = exit_status;
			free(line);
			reset_iteration(&shell);
			continue;
		}
		printf("\n[tokens_list_debug]:\n");	// for debugging, to be deleted
		print_token_list(&shell.tokens);	// for debugging, to be deleted


		exit_status = expand_tokens(&shell.tokens, &shell.env_vars, shell.exit_status);
		if (exit_status != ES_SUCCESS)
		{
			shell.exit_status = exit_status;
			free(line);
			reset_iteration(&shell);
			continue;
		}
		printf("\n[expanded_tokens_list_debug]:\n"); // for debugging, to be deleted
		print_token_list(&shell.tokens);	// for debugging, to be deleted


		exit_status = build_pipeline_from_tokens(&shell);
		if (exit_status != ES_SUCCESS)
		{
			shell.exit_status = exit_status;
			free(line);
			reset_iteration(&shell);
			continue;
		}
		printf("\n[pipelines_debug]:\n");	// for debugging, to be deleted
		print_pipe_line(shell.pipeline);	// for debugging, to be deleted

		g_sigint = 0;

		if (!process_heredoc(shell.pipeline, &shell.env_vars, shell.exit_status))
		{
			if (g_sigint)
			{
				g_sigint = 0;
				shell.exit_status = 130;
			}
			else
			{
				shell.exit_status = 1;
			}
			free_tokens(&shell.tokens);
			free(line);
			continue;
		}

		check_command_type_and_execute(&shell);	// Exec testing starts here
		shell.pipeline = NULL;
		free_tokens(&shell.tokens);
//		debug_print_heredoc_files(shell.pipeline); // for debugging, to be deleted

		// check_command_type_and_execute(shell);	// Exec testing starts here
		free(line);
		reset_iteration(&shell);
	}
	shell_destroy(&shell);
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

/* static int debug_print_heredoc_files(t_pipeline *pipeline) // for debugging, to be deleted
{
	size_t i;
	int fd;
	char buffer[1024];
	ssize_t bytes;

	i = 0;
	while (i < pipeline->count)
	{
		if (pipeline->cmds[i].infile)
		{
			fd = open(pipeline->cmds[i].infile, O_RDONLY);
			if (fd < 0)
				return (0);

			while (true)
			{
				bytes = read(fd, buffer, sizeof(buffer));
				if (bytes < 0)
				{
					close(fd);
					return 0;
				}
				if (bytes == 0)
					break;

				write(1, buffer, bytes);
			}
			close(fd);
		}
		i++;
	}
	return (1);
} */
