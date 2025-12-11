#include "minishell.h"

static void init_command(t_command *cmd);
static int append_arg(t_command *cmd, char *arg);
static int append_cmd(t_pipeline *pl, t_command cmd);
static void free_cmd(t_command *cmd);
static void free_pipeline(t_pipeline *pl);
static int token_has_any_quotes(t_token *token);

int build_pipeline_from_tokens(t_shell *shell)
{
    t_pipeline *pl;
    t_command current_cmd;
    t_token *current;
    t_token *next;
    int cmd_started;
    char *tmp;
    
    if (!shell)
        return (0);
    
    pl = malloc(sizeof(*pl));
    if (!pl)
        // shell->exit_status = 2;
        return (0);

    pl->cmds = NULL;
    pl->count = 0;

    init_command(&current_cmd);

    cmd_started = 0;
    current = shell->tokens.head;

    while(current)
    {
        printf("DEBUG: TOK_TYPE=%d, TOK_STR='%s'\n", current->type, current->raw_str);
        next = current->next;
        if (current->type == TOK_WORD)
        {
            if (!cmd_started)
            {
                init_command(&current_cmd);
                cmd_started = 1;
            }
            if (!append_arg(&current_cmd, current->raw_str))
            {
                free_cmd(&current_cmd);
                free_pipeline(pl);
                shell->exit_status = 2;
                return (0);
            }
        }
        else if (current->type == TOK_REDIR_IN
                || current->type == TOK_REDIR_OUT
                || current->type == TOK_APPEND
                || current->type == TOK_HEREDOC)
        {
            if (!cmd_started)
            {
                init_command(&current_cmd);
                cmd_started = 1;
            }
            if (!next || next->type != TOK_WORD)
            {
                // syntax_error("expected filename after redirection");
                free_cmd(&current_cmd);
                free_pipeline(pl);
                shell->exit_status = 258;
                return (0);
            }
            if (current->type == TOK_REDIR_IN) // <
            {
                tmp = ft_strdup(next->raw_str);
                if (!tmp)
                {
                    free_cmd(&current_cmd);
                    free_pipeline(pl);
                    shell->exit_status = 2;
                    return (0);
                }
                free(current_cmd.infile);
                current_cmd.infile = tmp;
            }
            else if (current->type == TOK_HEREDOC) // <<
            {
                tmp = ft_strdup(next->raw_str);
                if (!tmp)
                {
                    free_cmd(&current_cmd);
                    free_pipeline(pl);
                    shell->exit_status = 2;
                    return (0);
                }
                free(current_cmd.heredoc_limiter);
                current_cmd.heredoc_limiter = tmp;
                current_cmd.has_heredoc = 1;
                if (token_has_any_quotes(next))
                    current_cmd.heredoc_expand_needed = 0;
                else
                    current_cmd.heredoc_expand_needed = 1;
            }
            else
            {
                tmp = ft_strdup(next->raw_str);
                if (!tmp)
                {
                    free_cmd(&current_cmd);
                    free_pipeline(pl);
                    shell->exit_status = 2;
                    return (0);
                }
                free(current_cmd.outfile);
                current_cmd.outfile = tmp;

                if (current->type == TOK_REDIR_OUT) // >
                    current_cmd.append = 0;
                if (current->type == TOK_APPEND) // >>
                    current_cmd.append = 1;
            }
            current = next;
        }
        else if (current->type == TOK_PIPE)
        {
            if (!cmd_started || !next)
            {
                // syntax_error();
                free_cmd(&current_cmd);
                free_pipeline(pl);
                shell->exit_status = 258;
                return (0);
            }
            if (!append_cmd(pl, current_cmd))
            {
                free_cmd(&current_cmd);
                free_pipeline(pl);
                shell->exit_status = 2;
                return (0);
            }
            cmd_started = 0;
        }
        else
        {
            free_cmd(&current_cmd);
            free_pipeline(pl);
            shell->exit_status = 258;
            return (0);
        }
        current = current->next;
    }

    if (cmd_started)
    {
        if (!append_cmd(pl, current_cmd))
        {
            free_cmd(&current_cmd);
            free_pipeline(pl);
            shell->exit_status = 2;
            return (0);
        }
    }

    if (pl->count == 0)
    {
        free_pipeline(pl);
        free_cmd(&current_cmd);
        shell->exit_status = 2;
        return (0);
    }

    shell->pipeline = pl;
    return (1);
}

static void init_command(t_command *cmd)
{
    cmd->argv = NULL;
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->heredoc_limiter = NULL;
    cmd->append = 0;
    cmd->has_heredoc = 0;
    cmd->heredoc_expand_needed = 0;
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

static int append_cmd(t_pipeline *pl, t_command cmd)
{
    t_command *new_cmds;
    size_t i;

    i = 0;
    new_cmds = (t_command *)malloc(sizeof(* new_cmds) * (pl->count + 1));

    while (i < pl->count)
    {
        new_cmds[i] = pl->cmds[i];
        i++;
    }
    new_cmds[pl->count] = cmd;

    free(pl->cmds);

    pl->cmds = new_cmds;
    pl->count++;

    return (1);
}

static void free_cmd(t_command *cmd)
{
    size_t argc;

    if (cmd->argv)
    {
        argc = 0;
        while (cmd->argv[argc])
        {
            free(cmd->argv[argc]);
            argc++;
        }
    }
    free(cmd->infile);
    free(cmd->outfile);
    free(cmd->heredoc_limiter);
    init_command(cmd);
}

static void free_pipeline(t_pipeline *pl)
{
    size_t i;

    if (!pl)
        return;
    
    i = 0;
    while (i < pl->count)
    {
        free_cmd(&pl->cmds[i]);
        i++;
    }
    free(pl->cmds);
    free(pl);
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
