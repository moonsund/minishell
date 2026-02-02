#include "minishell.h"

static void print_token_list(t_token_list *list);
static void print_pipeline(t_pipeline *pipeline);
// static int debug_print_heredoc_files(t_pipeline *pipeline);

int main(int argc, char **argv, char **envp)
{
	char	*line;
	t_shell	shell;
	int		exit_status;

	(void)argc;
	(void)argv;
	exit_status = 0;
	if (!init_shell(&shell, envp))
	{
		err_print(ES_GENERAL, "init_shell");
			return (EXIT_FAILURE);
	}
	setup_signals();

	while(true)
	{
		line = readline("minishell> ");

		if (!line/*  || ft_strcmp(line, "exit") == 0 */) // Ctrl+D (EOF) or exit command
		{
			// printf("exit\n");					// Doesn't need to be printed
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
			free(line);
			reset_iteration(&shell);
			shell.exit_status = exit_status;
			continue;
		}
		// printf("\n[tokens_list_debug]:\n");	// for debugging, to be deleted
		// print_token_list(&shell.tokens);	// for debugging, to be deleted


		exit_status = expand_tokens(&shell.tokens, &shell.env_vars, shell.exit_status);
		if (exit_status != ES_SUCCESS)
		{
			free(line);
			reset_iteration(&shell);
			shell.exit_status = exit_status;
			continue;
		}
//		printf("\n[expanded_tokens_list_debug]:\n"); // for debugging, to be deleted
//		print_token_list(&shell.tokens);	// for debugging, to be deleted


		exit_status = build_pipeline_from_tokens(&shell);
		if (exit_status != ES_SUCCESS)
		{
			free(line);
			reset_iteration(&shell);
			shell.exit_status = exit_status;
			continue;
		}
		// printf("\n[pipelines_debug]:\n");	// for debugging, to be deleted
		// print_pipeline(shell.pipeline);	// for debugging, to be deleted

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

		shell.exit_status = execute_pipeline(&shell);
		free(line);
		reset_iteration(&shell);
		continue;

		shell.pipeline = NULL; // to be deleted
		free_tokens(&shell.tokens); // to be deleted

		// debug_print_heredoc_files(shell.pipeline); // for debugging, to be deleted
	}

	// if(ft_strcmp(minishell->pipeline->cmds->infile, ".heredoc_0") == 0)			// Free the heredoc file ? - TBC w/ Leon
	// {
	// 	char	*heredoc_file = build_path(".heredoc_0");
	// 	unlink(heredoc_file);
	// 	free(heredoc_file);
	// }

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

static const char *redir_type_to_str(t_redir_type type)
{
	if (type == R_IN)
		return ("<");
	if (type == R_OUT)
		return (">");
	if (type == R_APPEND)
		return (">>");
	if (type == R_HEREDOC)
		return ("<<");
	return ("?");
}

static void	print_pipeline(t_pipeline *pipeline) /* for debugging, to be deleted */
{
	size_t	i;
	size_t	j;
	t_redir	*r;

	if (!pipeline || !pipeline->cmds)
		return ;

	printf("[pipelines_debug]: count=%zu\n", pipeline->count);
	for (i = 0; i < pipeline->count; i++)
	{
		printf("Command %zu:\n", i);

		if (!pipeline->cmds[i].argv || !pipeline->cmds[i].argv[0])
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

		r = pipeline->cmds[i].redirs;
		if (!r)
			printf("  (no redirs)\n");
		else
		{
			j = 0;
			while (r)
			{
				printf("  redir[%zu]: type = %s fd = %d target = %s",
					j,
					redir_type_to_str(r->type),
					r->fd,
					(r->target ? r->target : "(null)"));
				if (r->type == R_HEREDOC)
					printf(" expand = %d", r->expand);
				printf("\n");
				r = r->next;
				j++;
			}
		}
	}
}

/*
static int debug_print_heredoc_files(t_pipeline *pipeline) // for debugging, to be deleted
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
}
*/
