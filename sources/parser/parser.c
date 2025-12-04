#include "minishell.h"

static void init_command(t_command *cmd);
static int append_arg(t_command *cmd, char *arg);
static int append_cmd(t_pipeline *pl, t_command cmd);
static void free_cmd(t_command *cmd);
static void free_pipeline(t_pipeline *pl);

int build_pipeline_from_tokens(t_shell *shell)
{
    t_pipeline *pl;
    t_command current_cmd;
    t_token *cur;
    t_token *cur_next;
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
    cur = shell->tokens.head;

    while(cur)
    {
        printf("TYPE=%d, STR='%s'\n", cur->type, cur->raw_str);
        cur_next = cur->next;
        if (cur->type == TOK_WORD)
        {
            if (!cmd_started)
            {
                init_command(&current_cmd);
                cmd_started = 1;
            }
            if (!append_arg(&current_cmd, cur->raw_str))
            {
                free_cmd(&current_cmd);
                free_pipeline(pl);
                shell->exit_status = 2;
                return (0);
            }
        }
        // else if (cur->type == TOK_REDIR_IN, cur->type == TOK_REDIR_OUT, cur->type == TOK_HEREDOC, cur->type == TOK_APPEND)
        else if (cur->type == TOK_REDIR_IN
                || cur->type == TOK_REDIR_OUT
                || cur->type == TOK_APPEND)
        {
            if (!cmd_started)
            {
                init_command(&current_cmd);
                cmd_started = 1;
            }
            if (!cur_next || cur_next->type != TOK_WORD)
            {
                // syntax_error("expected filename after redirection");
                free_cmd(&current_cmd);
                free_pipeline(pl);
                shell->exit_status = 258;
                return (0);
            }
            if (cur->type == TOK_REDIR_IN) // <
            {
                tmp = ft_strdup(cur_next->raw_str);
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
            else
            {
                free(current_cmd.outfile);
                tmp = ft_strdup(cur_next->raw_str);
                if (!tmp)
                {
                    free_cmd(&current_cmd);
                    free_pipeline(pl);
                    shell->exit_status = 2;
                    return (0);
                }
                free(current_cmd.infile);
                current_cmd.infile = tmp;
                
                if (cur->type == TOK_REDIR_OUT) // >
                    current_cmd.append = 0;
                if (cur->type == TOK_APPEND) // >>
                    current_cmd.append = 1;
            }
        }
        else if (cur->type == TOK_PIPE)
        {
            if (!cmd_started || !cur_next)
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
        cur = cur->next;
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
