#include "minishell.h"

int append_cmd(t_pipeline *pl, t_command cmd);

int build_pipeline_from_tokens(t_shell *shell)
{
    t_parser_context ctx;
    t_pipeline *pl;

    if (!shell)
        return (0);

    init_parser_context(&ctx);

    pl = init_pipeline();
    if (!pl)
    {
        shell->exit_status = 2;
        return (0);
    }

    if (!process_tokens(pl, &shell->tokens, &ctx, &shell->exit_status))
    {
        free_cmd(&ctx.current_cmd);
        free_pipeline(pl);
        return (0);
    }

    if (pl->count == 0)
    {
        free_cmd(&ctx.current_cmd);
        free_pipeline(pl);
        shell->exit_status = 2;
        return (0);
    }

    shell->pipeline = pl;
    return (1);
}

int append_cmd(t_pipeline *pl, t_command cmd)
{
    t_command *new_cmds;
    size_t i;

    new_cmds = malloc(sizeof(* new_cmds) * (pl->count + 1));
    if (!new_cmds)
    {
        err_print(ERR_SYS, "append command");
        return (0);
    }

    i = 0;
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
