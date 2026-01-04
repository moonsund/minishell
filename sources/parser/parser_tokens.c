#include "minishell.h"

static int process_word_token(t_parser_context *ctx);
static int append_arg(t_command *cmd, char *arg);
static t_exit_status process_pipe_token(t_pipeline *pl, t_parser_context *ctx);
static t_exit_status process_redir_tokens(t_parser_context *ctx);
static int process_redir_in_token(char **tmp, char *raw_str, char **infile);
static int process_redir_out_append_tokens(t_parser_context *ctx);
static int token_has_any_quotes(t_token *token);
static int process_heredoc_token(t_parser_context *ctx);

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

    if (ctx->cmd_started) // the last command after the pipe
    {
        if (ctx->current_cmd.argv == NULL || ctx->current_cmd.argv[0] == NULL)
        {
            err_print(ES_SYNTAX, "near 'newline'");
            return (ES_SYNTAX);
        }
        if (!append_cmd(pl, ctx->current_cmd))
            return (ES_GENERAL);
    }
    return (ES_SUCCESS);
}

static int process_word_token(t_parser_context *ctx)
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

static int append_arg(t_command *cmd, char *arg)
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
        err_print(ES_SYNTAX, "parse error near '|'");
        return (ES_SYNTAX);
    }
    if (!append_cmd(pl, ctx->current_cmd))
        return (ES_GENERAL);

    init_command(&ctx->current_cmd);
    ctx->cmd_started = 0;
    return (ES_SUCCESS);
}

static t_exit_status process_redir_tokens(t_parser_context *ctx)
{
    if (!ctx->cmd_started)
    {
        init_command(&ctx->current_cmd);
        ctx->cmd_started = 1;
    }

    if (!ctx->next || ctx->next->type != TOK_WORD)
    {
        err_print(ES_SYNTAX, "expected filename or limiter"); 
        return (ES_SYNTAX);
    }

    if (ctx->current->type == TOK_REDIR_IN) // <
    {
        if (!process_redir_in_token(&ctx->tmp, ctx->next->raw_str, &ctx->current_cmd.infile))
            return (ES_GENERAL);
    }
    else if (ctx->current->type == TOK_HEREDOC) // <<
    {
        if (!process_heredoc_token(ctx))
            return (ES_GENERAL);
    }
    else
    {
        if (!process_redir_out_append_tokens(ctx))
            return (ES_GENERAL);
    }
    ctx->current = ctx->next;
    return (ES_SUCCESS);
}

static int process_redir_in_token(char **tmp, char *raw_str, char **infile)
{
    *tmp = ft_strdup(raw_str);
    if (!*tmp)
    {
        err_malloc_print("parser: redir in token");
        return (0);
    }
    free(*infile);
    *infile = *tmp;
    *tmp = NULL;
    return (1);
}

static int process_heredoc_token(t_parser_context *ctx)
{
    ctx->tmp = ft_strdup(ctx->next->raw_str);
    if (!ctx->tmp)
    {
        err_malloc_print("parser: heredoc token");
        return (0);
    }
    free(ctx->current_cmd.heredoc_limiter);
    ctx->current_cmd.heredoc_limiter = ctx->tmp;
    ctx->tmp = NULL;
    ctx->current_cmd.has_heredoc = 1;
    if (token_has_any_quotes(ctx->next))
        ctx->current_cmd.heredoc_expand_needed = 0;
    else
        ctx->current_cmd.heredoc_expand_needed = 1;
    return (1);
}

static int token_has_any_quotes(t_token *token)
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

static int process_redir_out_append_tokens(t_parser_context *ctx)
{
    ctx->tmp = ft_strdup(ctx->next->raw_str);
    if (!ctx->tmp)
    {
        err_malloc_print("parser: redir out token");
        return (0);
    }
    free(ctx->current_cmd.outfile);
    ctx->current_cmd.outfile = ctx->tmp;
    ctx->tmp = NULL;

    if (ctx->current->type == TOK_REDIR_OUT) // >
        ctx->current_cmd.append = 0;
    else if (ctx->current->type == TOK_APPEND) // >>
        ctx->current_cmd.append = 1;
    return (1);
}