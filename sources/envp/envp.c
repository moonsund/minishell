#include "minishell.h"

int    init_env_var_list(t_env_var_list *list, char **envp);
t_var  *find_var(t_env_var_list *list, const char *name);
int     set_var(t_env_var_list *list, const char *name, const char *value);   // export
int     unset_var(t_env_var_list *list, const char *name);                    // unset
char   *get_var_value(t_env_var_list *var_list, const char *var);                // my_getenv
char  **build_envp(t_env_var_list *list);                                     // before execve
void    free_var_list(t_env_var_list *list);
static void free_envp_partial(char **envp, size_t used);

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

t_var   *find_var(t_env_var_list *list, const char *name)
{
    t_var *cur;

    if (!list || !name)
        return (NULL);

    cur = list->head;
    while (cur != NULL)
    {
        if (ft_strcmp(cur->name, name) == 0)
            return (cur);
        cur = cur->next;
    }
    return (NULL);
}

int set_var(t_env_var_list *list, const char *name, const char *value)
{
    t_var *variable;
    t_var *new_variable;
    char *new_value;
    char *new_name;
    
    if (!list || !name)
        return (0);

    variable = find_var(list, name);

    if (variable)
    {
        if (!value)
            return (1);

        new_value = ft_strdup(value);
        if (!new_value)
            return (0);

        free(variable->value);
        variable->value = new_value;
        return (1);
    }

    new_variable = malloc(sizeof(*new_variable));
    if (!new_variable)
        return (0);

    new_name = ft_strdup(name);
    if (!new_name)
    {
        free(new_variable);
        return (0);
    }

    if (!value)
        new_value = ft_strdup("");
    else
        new_value = ft_strdup(value);

    if (!new_value)
    {
        free(new_name);
        free(new_variable);
        return (0);
    }

    new_variable->name = new_name;
    new_variable->value = new_value;
    new_variable->next = NULL;

    if (!list->head)
    {
        list->head = new_variable;
        list->tail = new_variable;
    }
    else
    {
        list->tail->next = new_variable;
        list->tail = new_variable;
    }
    list->count++;

    return (1);
}

int unset_var(t_env_var_list *list, const char *name)
{
    t_var *prev;
    t_var *cur;

    if (!list || !name)
        return (0);

    prev = NULL;
    cur = list->head;

    while (cur != NULL)
    {
        if (ft_strcmp(cur->name, name) == 0)
        {
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    if (!cur)
        return (1);
    else
    {
        if (prev == NULL)
            list->head = cur->next;
        else
        {
            prev->next = cur->next;
            if (cur == list->tail)
                list->tail = prev;
        }
    }
    free(cur->name);
    free(cur->value);
    free(cur);
    list->count--;
    if (list->count == 0)
        list->tail = NULL;
    return (1);
}

char   *get_var_value(t_env_var_list *var_list, const char *var)
{
    t_var *cur;

    if (!var_list || !var)
        return (NULL);

    cur = var_list->head;

    while (cur != NULL)
    {
        if (ft_strcmp(cur->name, var) == 0)
            return (cur->value);
        cur = cur->next;
    }
    return (NULL);
}

char  **build_envp(t_env_var_list *list)
{
    char **envp;
    size_t i;
    char *tmp;
    char *variable;
    t_var *cur;

    if (!list)
        return (NULL);
    
    cur = list->head;
    i = 0;
    envp = malloc(sizeof(*envp) * (list->count + 1));
    if (!envp)
        return (NULL);

    while (cur != NULL)
    {
        tmp = ft_strjoin(cur->name, "=");
        if (!tmp)
        {
            free_envp_partial(envp, i);
            return (NULL);
        }

        if (cur->value == NULL)
            variable = ft_strjoin(tmp, "");
        else
            variable = ft_strjoin(tmp, cur->value);
        if (!variable)
        {
            free(tmp);
            free_envp_partial(envp, i);
            return (NULL);
        }
        free(tmp);
        envp[i] = variable;
        i++;
        cur = cur->next;
    }
    envp[i] = NULL;
    return (envp);
}

static void free_envp_partial(char **envp, size_t used)
{
    size_t j;

    if (!envp)
        return;

    j = 0;
    while (j < used)
    {
        free(envp[j]);
        j++;
    }
    free(envp);
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