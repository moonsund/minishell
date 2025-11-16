#include "minishell.h"
#include "libft.h"

// Put all functions signatures here when done (Leo's way)

// liste chainee - A convertir en char ** pour execve sinon pas pratique
// Implémenter $ later (voir avec Leo pour decider ensemble qui s'en occupe)
t_env	**build_environment(void)
{
	t_env	**environment;
	environment = malloc(sizeof(t_env*));						// Ⓜ️
	t_env	*new;

	char *all_vars[] = {"HOME", "USER", "LANG", "HOME"};		// Added twice HOME for testing
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
		ft_printf("Variable Name and/or Data missing\n");
		return (NULL);
	}
	new_env_var = malloc(sizeof(t_env));						// Ⓜ️
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
		if(str_comp(backup_ptr->variable_name, new->variable_name) == 0)
		{
			free(backup_ptr->variable_data);
			backup_ptr->variable_data = ft_strdup(new->variable_data);
			free(new->variable_name);
			free(new->variable_data);
			free(new);
			new = NULL;
			return;
		}
		backup_ptr = backup_ptr->next;
	}
	backup_ptr->next = new;
}

// Probably not fully set up - Test & Check
void	delete_env_var(t_env **head, char *var_to_delete, void (*del_string)(void *))
{
	if (!var_to_delete || !del_string || head == NULL)
		return ;
	t_env	*copy_head;
	t_env	*backup_previous;

	copy_head = *head;
	backup_previous = NULL;

	while (copy_head != NULL)
	{
		if(str_comp(copy_head->variable_name, var_to_delete) == 0)
		{
			backup_previous->next = copy_head->next;
			del_string(copy_head->variable_name);
			del_string(copy_head->variable_data);
			free(copy_head);
			copy_head = NULL;
			return;
		}
		backup_previous = copy_head;
		copy_head = copy_head->next;
	}
	// Leaving the while loop = nothing to delete / env var not found
	ft_printf("Environment variable not found\n");
}

void	del_string(char *param)
{
	if (param)
		free(param);
}
int		str_comp(char *s1, char *s2)
{
	int	i = 0;
	if(!s1 || !s2)
		return(-1);
	if(ft_strlen(s1) != ft_strlen(s2))
		return(-1);

	while (s1[i] != '\0')
	{
		if(s1[i] != s2[i])
		{
			return(-1);
		}
		i++;
	}
	return(0);
}

void	free_all_vars(t_env **head)
{
	if (head == NULL)
		return ;
	t_env	*copy_head;
	t_env	*backup_next;
	copy_head = *head;

	while (copy_head != NULL)
	{
		backup_next = copy_head->next;
		del_string(copy_head->variable_name);
		del_string(copy_head->variable_data);
		free(copy_head);						// not necessary if node hasn't been malloc'ed
		copy_head = copy_head->next;
	}
}
