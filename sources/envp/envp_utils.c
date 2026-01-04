#include "minishell.h"

void free_envp_partial(char **envp, size_t used);

void free_envp_partial(char **envp, size_t used)
{
    size_t i;

    if (!envp)
        return;

    i = 0;
    while (i < used)
    {
        free(envp[i]);
        i++;
    }
    free(envp);
}