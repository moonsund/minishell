#include "/home/schappuy/00_Root/08_Minishell/includes/minishell.h"

t_pipeline *init_pipeline();
void init_parser_context(t_parser_context *ctx);
void init_command(t_command *cmd);

t_pipeline *init_pipeline()
{
    t_pipeline *pl;

    pl = malloc(sizeof(*pl));
    if (!pl)
    {
        err_malloc_print("init_pipeline");
        return (NULL);
    }

    pl->cmds = NULL;
    pl->count = 0;

    return (pl);
}

void init_parser_context(t_parser_context *ctx)
{
    init_command(&ctx->current_cmd);
    ctx->current = NULL;
    ctx->next = NULL;
    ctx->tmp = NULL;
    ctx->cmd_started = 0;
}

void init_command(t_command *cmd)
{
    cmd->argv = NULL;
    cmd->argc = 0;
    cmd->redirs = NULL;
}
