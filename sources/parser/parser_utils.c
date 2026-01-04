#include "minishell.h"

void free_cmd(t_command *cmd);

void free_cmd(t_command *cmd)
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
    free(cmd->argv);
    free(cmd->infile);
    free(cmd->outfile);
    free(cmd->heredoc_limiter);
    init_command(cmd);
}