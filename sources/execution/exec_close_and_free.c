#include "minishell.h"
#include "libft.h"

void	close_and_set_to_neg(int *fd);
void	close_if_valid(int fd);
void	free_envp(t_env_var_list *list);
void	free_strings_array(char **array);

// Not used anymore - Replaced by the one below
void	close_and_set_to_neg(int *fd)
{
	if (*fd && *fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}

// Ok to pass the fd rather than a pointer to it ? TBC
// Would it be smart to set to -1  after closing ? TBD
void	close_if_valid(int fd)
{
	if (fd >= 0)
		close(fd);
}

void	free_envp(t_env_var_list *list)
{
	int	i;

	i = list->count;
	if(!list)
		return;

	while (i > 0)
	{
		unset_var(list, list->head->name);
		i--;
	}
}

void	free_strings_array(char **array)
{
	int	i;

	i = 0;
	if(!array)
		return;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}
