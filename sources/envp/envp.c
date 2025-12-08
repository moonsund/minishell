#include "minishell.h"

int    init_env_var_list(t_env_var_list *list, char **envp);
// t_var  *find_var(t_env_var_list *list, const char *name);
// int     set_var(t_env_var_list *list, const char *name, const char *value);   // export
// int     unset_var(t_env_var_list *list, const char *name);                    // unset
char   *get_var_value(t_env_var_list *var_list, const char *var);                // my_getenv
// char  **build_envp(t_env_var_list *list);                                     // before execve
void    free_var_list(t_env_var_list *list);

int    init_env_var_list(t_env_var_list *list, char **envp)
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
            free_var_list(list);
            return (0);
        }

        var->name = ft_substr(envp[i], 0, sep_ptr - envp[i]);
        var->value = ft_strdup(sep_ptr + 1);
        if (!var->name || !var->value)
        {
            free(var->name);
            free(var->value);
            free(var);
            free_var_list(list);
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

char   *get_var_value(t_env_var_list *var_list, const char *var)
{
    t_var *cur;

    if (!var_list || !var)
        return (NULL);

    cur = var_list->head;

    while (cur)
    {
        if (ft_strcmp(cur->name, var) == 0)
            return (cur->value);
        cur = cur->next;
    }
    return (NULL);
}

void    free_var_list(t_env_var_list *vars)
{
    t_var *cur;
	t_var *next;

	if (!vars)
		return ;

	cur = vars->head;
	while (cur != NULL)
	{
		next = cur->next;
		free(cur->name);
		free(cur->value);
        free(cur);
		cur = next;
	}
	vars->head = NULL;
    vars->tail = NULL;
	vars->count = 0;
}