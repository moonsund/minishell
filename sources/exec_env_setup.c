#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

// liste chainee - A convertir en char ** pour execve sinon pas pratique
// Implémenter $ later (voir avec Leo pour decider ensemble qui s'en occupe)
t_env	**build_environment(void)
{
	t_env	**environment;
	environment = malloc(sizeof(t_env*));
	t_env	*new;

	char *all_vars[] = {"HOME", "USER", "LANG", "HOME"};
	int	size_tab = (sizeof(all_vars) / sizeof(all_vars[0]));
	int i = 0;
	while (i < size_tab)
	{
		new = create_new_environment_variable(all_vars[i], getenv(all_vars[i]));
		add_env_var_to_list(environment, new);
		i++;
	}
	return(environment);
}

t_env	*create_new_environment_variable(char *key, char *value)
{
	t_env	*new_env_var;
	if (!key || !value)
	{
		printf("Variable Name and/or Data missing\n");
		return (NULL);
	}
	new_env_var = malloc(sizeof(t_env));
	if (!new_env_var)
		return (NULL);
	new_env_var->variable_name = ft_strdup(key);
	new_env_var->variable_data = ft_strdup(value);
	new_env_var->next = NULL;
	return (new_env_var);
}

void	add_env_var_to_list(t_env **head, t_env *new)
{
	t_env	*last;

	if (*head == NULL)
	{
		*head = new;
		return ;
	}
	t_env	*backup_ptr = *head;
	while (backup_ptr->next != NULL)
	{
		if(ft_memcmp(backup_ptr->variable_name, new->variable_name, ft_strlen(new->variable_name)) == 0)
		{
			free(backup_ptr->variable_data);
			backup_ptr->variable_data = ft_strdup(new->variable_data);
			free(new->variable_name);
			free(new->variable_data);
			// free node if it has been malloc'ed
			return;
		}
		backup_ptr = backup_ptr->next;
	}
	backup_ptr->next = new;
}

// Probably not fully set up - Test & Check
void	delete_env_var_content(t_env *env_var, void (*del_string)(void *))
{
	if (!env_var || !del_string)
		return ;
	if (env_var)
	{
		del_string(env_var->variable_name);
		del_string(env_var->variable_data);
		free(env_var);
		env_var = NULL;
	}
}

void	del_string(char *param)
{
	if (param)
		free(param);
}
