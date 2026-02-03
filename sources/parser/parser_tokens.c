#include "minishell.h"

int process_word_token(t_parser_context *ctx);
int append_arg(t_command *cmd, char *arg);
static t_exit_status process_pipe_token(t_pipeline *pl, t_parser_context *ctx);
static t_exit_status process_redir_tokens(t_parser_context *ctx);
int token_has_any_quotes(t_token *token);
int redir_push_back(t_redir **lst, t_redir *node);
static t_redir *init_redirect(t_parser_context *ctx);

t_exit_status process_tokens(t_pipeline *pl, t_token_list *list, t_parser_context *ctx)
{
	t_exit_status exit_status;
	ctx->current = list->head;

	while(ctx->current)
	{
		ctx->next = ctx->current->next;
		if (ctx->current->type == TOK_WORD)
		{
			if (!process_word_token(ctx))
				return (ES_GENERAL);
		}
		else if (ctx->current->type == TOK_PIPE) // |
		{
			exit_status = process_pipe_token(pl, ctx);
			if (exit_status != ES_SUCCESS)
				return (exit_status);
		}
		else // < << > >>
		{
			exit_status = process_redir_tokens(ctx);
			if (exit_status != ES_SUCCESS)
				return (exit_status);
		}
		ctx->current = ctx->current->next;
	}

	if (ctx->cmd_started) // last command (after loop)
	{
		int has_argv;
		int has_redirs;

		has_argv = (ctx->current_cmd.argv && ctx->current_cmd.argv[0]);
		has_redirs = (ctx->current_cmd.redirs != NULL);

		/* real "empty command" => syntax error */
		if (!has_argv && !has_redirs)
		{
			err_print(ES_INVALID_USAGE, "near 'newline'");
			return (ES_INVALID_USAGE);
		}

		/* allow commands like: >out  or  <in >out */
		if (!append_cmd(pl, ctx->current_cmd))
			return (ES_GENERAL);
	}

	return (ES_SUCCESS);
}

int process_word_token(t_parser_context *ctx)
{
	if (!ctx->cmd_started)
	{
		init_command(&ctx->current_cmd);
		ctx->cmd_started = 1;
	}
	if (!append_arg(&ctx->current_cmd, ctx->current->raw_str))
	{
		err_malloc_print("parser: append arg");
		return (0);
	}
	return (1);
}

int append_arg(t_command *cmd, char *arg)
{
	char **new_argv;
	char *dup;
	size_t argc;
	size_t i;

	if (!cmd)
		return (0);

	if (!arg)
		return (1);

	argc = 0;
	if (cmd->argv)
	{
		while(cmd->argv[argc])
			argc++;
	}

	new_argv = (char **)malloc(sizeof(* new_argv) * (argc + 2));
	if (!new_argv)
		return (0);

	i = 0;
	while (i < argc)
	{
		new_argv[i] = cmd->argv[i];
		i++;
	}

	dup = ft_strdup(arg);
	if (!dup)
	{
		free(new_argv);
		return (0);
	}

	new_argv[argc] = dup;
	new_argv[argc + 1] = NULL;

	free(cmd->argv);
	cmd->argv = new_argv;
	return (1);
}

static t_exit_status process_pipe_token(t_pipeline *pl, t_parser_context *ctx)
{
	if (!ctx->cmd_started || !ctx->next || ctx->current_cmd.argv == NULL || ctx->current_cmd.argv[0] == NULL)
	{
		err_print(ES_INVALID_USAGE, "parse error near '|'");
		return (ES_INVALID_USAGE);
	}
	if (!append_cmd(pl, ctx->current_cmd))
		return (ES_GENERAL);

	init_command(&ctx->current_cmd);
	ctx->cmd_started = 0;
	return (ES_SUCCESS);
}

static t_exit_status process_redir_tokens(t_parser_context *ctx)
{
	t_redir *redir_node;

	if (!ctx->cmd_started)
	{
		init_command(&ctx->current_cmd);
		ctx->cmd_started = 1;
	}

	if (!ctx->next || ctx->next->type != TOK_WORD)
	{
		err_print(ES_INVALID_USAGE, "expected filename or limiter");
		return (ES_INVALID_USAGE);
	}

	redir_node = init_redirect(ctx);
	if (!redir_node)
	{
		err_malloc_print("parser: redir node");
		return (ES_GENERAL);
	}

	if (!redir_push_back(&ctx->current_cmd.redirs, redir_node))
	{
		free(redir_node->target);
		free(redir_node);
		return (ES_GENERAL);
	}

	ctx->current = ctx->next;
	return (ES_SUCCESS);
}

static t_redir *init_redirect(t_parser_context *ctx)
{
	t_redir *redir;
	t_token_type token_type = ctx->current->type;

	redir = malloc(sizeof(* redir));
	if (!redir)
		return NULL;

	redir->fd = 0;
	redir->expand = 0;
	redir->next = NULL;

	if (token_type == TOK_REDIR_IN)     // <
	{
		redir->type = R_IN;
		redir->fd = 0;
	}

	else if (token_type == TOK_HEREDOC)     // <<
	{
		redir->type = R_HEREDOC;
		if (!token_has_any_quotes(ctx->next))
			redir->expand = 1;
	}
	else if (token_type == TOK_REDIR_OUT)	// >
	{
		redir->type = R_OUT;
		redir->fd = 1;
	}
	else 	// >>
	{
		redir->type = R_APPEND;
		redir->fd = 1;
	}

	redir->target = ft_strdup(ctx->next->raw_str);
	if (!redir->target)
	{
		free(redir);
		return (NULL);
	}
	return redir;
}

int token_has_any_quotes(t_token *token)
{
	size_t i;

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

int redir_push_back(t_redir **lst, t_redir *node)
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
