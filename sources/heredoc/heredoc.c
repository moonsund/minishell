#include "minishell.h"


int process_heredocs(t_shell *shell)
{
    size_t  i;
    char    *line;

    if (!shell || !shell->pipeline)
        return (0);

    i = 0;
    while (i < shell->pipeline->count)
    {
        if (shell->pipeline->cmds[i].has_heredoc)
        {
            while (true)
            {
                line = readline("heredoc> ");
                if (!line)
                    return (0);

                if (shell->pipeline->cmds[i].heredoc_limiter
                    && ft_strcmp(line, shell->pipeline->cmds[i].heredoc_limiter) == 0)
                {
                    free(line);
                    break;
                }

                if (!is_empty(line))
                    add_history(line);

                // TODO: write_in_function()

                free(line);
            }
        }
        i++;
    }
    return (1);
}