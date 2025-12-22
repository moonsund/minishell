#include "minishell.h"

int init_shell(t_shell *shell, char **envp);
static int init_env_var_list(t_env_var_list *list, char **envp);

int init_shell(t_shell *shell, char **envp)
{
    shell->exit_status = 1;
	shell->pipeline = NULL;
    shell->tokens.count = 0;
    shell->tokens.head = NULL;
    shell->tokens.tail = NULL;

	if (!init_env_var_list(&shell->env_vars, envp))
		return (0);
	return (1);
}

static int init_env_var_list(t_env_var_list *list, char **envp)
{
	t_var *var;
	size_t i;
    char *sep_ptr;

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;

    if (!envp)
        return (1);
	
    i = 0;
    while (envp[i])
    {
        sep_ptr = ft_strchr(envp[i], '=');
        if (!sep_ptr)
        {
            i++;
            continue;
        }

        var = malloc(sizeof(*var));
        if (!var)
        {
            free_env_var_list(list);
            return (0);
        }

        var->name = ft_substr(envp[i], 0, sep_ptr - envp[i]);
        var->value = ft_strdup(sep_ptr + 1);
        if (!var->name || !var->value)
        {
            free(var->name);
            free(var->value);
            free(var);
            free_env_var_list(list);
            return (0);
        }
        
        var->next = NULL;

        if (list->head == NULL)
            list->head = var;
        else
            list->tail->next = var;
        list->tail = var;

        list->count++;
        i++;
    }
	return (1);
}