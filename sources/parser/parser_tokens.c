#include "minishell.h"

static int process_word_token(t_parser_context *ctx, int *exit_status);
static int append_arg(t_command *cmd, char *arg);
static int process_pipe_token(t_pipeline *pl, t_parser_context *ctx, int *exit_status);
static int process_redir_tokens(t_parser_context *ctx, int *exit_status);
static int process_redir_in_token(char **tmp, char *raw_str, char **infile);
static int process_redir_out_append_tokens(t_parser_context *ctx);
static int token_has_any_quotes(t_token *token);
static int process_heredoc_token(t_parser_context *ctx);

int process_tokens(t_pipeline *pl, t_token_list *list, t_parser_context *ctx, int *exit_status)
{
    ctx->current = list->head;

    while(ctx->current)
    {
        ctx->next = ctx->current->next;
        if (ctx->current->type == TOK_WORD)
        {
            if (!process_word_token(ctx, exit_status))
            {
                err_print(ERR_SYS, "process_tokens");
                return (0);
            }
        }
        else if (ctx->current->type == TOK_PIPE) // |
        {
            if (!process_pipe_token(pl, ctx, exit_status))
                return (0);
        }
        else // < << > >>
        {
            if (!process_redir_tokens(ctx, exit_status))
                return (0);
        }
        ctx->current = ctx->current->next;
    }

    if (ctx->cmd_started) // the last command after the pipe
    {
        if (ctx->current_cmd.argv == NULL || ctx->current_cmd.argv[0] == NULL)
        {
            err_print(ERR_SYNTAX, "near 'newline'");
            *exit_status = 258;
            return (0);
        }
        if (!append_cmd(pl, ctx->current_cmd))
        {
            *exit_status = 2;
            return (0);
        }
    }
    return (1);
}

static int process_word_token(t_parser_context *ctx, int *exit_status)
{
    if (!ctx->cmd_started)
    {
        init_command(&ctx->current_cmd);
        ctx->cmd_started = 1;
    }
    if (!append_arg(&ctx->current_cmd, ctx->current->raw_str))
    {
        *exit_status = 1;
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

static int process_pipe_token(t_pipeline *pl, t_parser_context *ctx, int *exit_status)
{
    if (!ctx->cmd_started || !ctx->next || ctx->current_cmd.argv == NULL || ctx->current_cmd.argv[0] == NULL)
    {
        err_print(ERR_SYNTAX, "parse error near '|'");
        *exit_status = 258;
        return (0);
    }
    if (!append_cmd(pl, ctx->current_cmd))
    {
        *exit_status = 2;
        return (0);
    }
    init_command(&ctx->current_cmd);
    ctx->cmd_started = 0;
    return (1);
}

static int process_redir_tokens(t_parser_context *ctx, int *exit_status)
{
    if (!ctx->cmd_started)
    {
        init_command(&ctx->current_cmd);
        ctx->cmd_started = 1;
    }

    if (!ctx->next || ctx->next->type != TOK_WORD)
    {
        err_print(ERR_SYNTAX, "expected filename or limiter"); 
        *exit_status = 258;
        return (0);
    }

    if (ctx->current->type == TOK_REDIR_IN) // <
    {
        if (!process_redir_in_token(&ctx->tmp, ctx->next->raw_str, &ctx->current_cmd.infile))
        {
            *exit_status = 1;
            return (0);
        }
    }
    else if (ctx->current->type == TOK_HEREDOC) // <<
    {
        if (!process_heredoc_token(ctx))
        {    
            *exit_status = 1;
            return (0);
        }
    }
    else
    {
        if (!process_redir_out_append_tokens(ctx))
        {
            *exit_status = 1;
            return (0);
        }
    }
    ctx->current = ctx->next;
    return (1);
}

static int process_redir_in_token(char **tmp, char *raw_str, char **infile)
{
    *tmp = ft_strdup(raw_str);
    if (!*tmp)
    {
        err_print(ERR_SYS, "process_redir_in_token");
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
        err_print(ERR_SYS, "process_heredoc_token");
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
        err_print(ERR_SYS, "process_redir_out_token");
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