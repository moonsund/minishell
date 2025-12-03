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
                free(current_cmd.infile);
                current_cmd.infile = ft_strdup(cur_next->raw_str);
                // malloc
            }
            else
            {
                free(current_cmd.outfile);
                current_cmd.outfile = ft_strdup(cur_next->raw_str);
                // malloc
                if (cur->type == TOK_REDIR_OUT) // >
                    current_cmd.append = 0;
                if (cur->type == TOK_APPEND) // >>
                    current_cmd.append = 1;
            }
        }
        else if (cur->type == TOK_PIPE)
        {
            if (!cmd_started || !cur_next || cur_next->type != TOK_WORD)
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
    cmd->args = NULL;
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->heredoc_limiter = NULL;
    cmd->append = 0;
}

static int append_arg(t_command *cmd, char *arg)
{
    size_t argc;
    char **new_argv;

    if (!arg)
        return (1);
    
    if (!cmd->args)
    {
        new_argv = (char **)malloc(sizeof(* new_argv) * 2);
        if (!new_argv)
            return (0);
        new_argv[0] = ft_strdup(arg);
        if (!new_argv[0])
        {
            free(new_argv);
            return (0);
        }
        new_argv[1] = NULL;
        cmd->args = new_argv;
        return (1);
    }
    argc = 0;
    while(cmd->args[argc])
        argc++;
    
    new_argv = (char **)realloc(cmd->args, sizeof(* new_argv) * (argc + 2)); // realloc
    // malloc
    
    new_argv[argc] = ft_strdup(arg);
    if (!new_argv[argc])
    {
        free(new_argv);
        return (0);
    }

    new_argv[argc + 1] = NULL;
    cmd->args = new_argv;
    return (1);
}

static int append_cmd(t_pipeline *pl, t_command cmd)
{
    t_command *new_cmds;

    new_cmds = (t_command *)realloc(pl->cmds, sizeof(* new_cmds) * (pl->count + 1)); // realloc
    // malloc
    pl->cmds = new_cmds;

    pl->cmds[pl->count] = cmd;
    pl->count++;

    init_command(&cmd);
    return (1);
}

static void free_cmd(t_command *cmd)
{
    size_t argc;

    if (cmd->args)
    {
        argc = 0;
        while (cmd->args[argc])
        {
            free(cmd->args[argc]);
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
