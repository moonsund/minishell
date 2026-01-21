#include "/home/schappuy/00_Root/08_Minishell/includes/minishell.h"
#include "libft.h"

void	replace_cmd_by_binary_path(char *cmd);

void	replace_cmd_by_binary_path(char *cmd)
{
	char	*first_arg;

	// ⬇️ Create binary path for the current command
	first_arg = ft_strjoin("/bin/", cmd);
	free(cmd);
	// ⬇️ Same address = easier to track for debug
	cmd = ft_strdup(first_arg);
	free(first_arg);
}
