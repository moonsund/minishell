#include "minishell.h"
#include "libft.h"
#include <unistd.h>

static int	is_builtin(const char *s)
{
	if (!s)
		return (0);
	if (ft_strcmp(s, "echo") == 0)
		return (1);
	if (ft_strcmp(s, "cd") == 0)
		return (1);
	if (ft_strcmp(s, "pwd") == 0)
		return (1);
	if (ft_strcmp(s, "export") == 0)
		return (1);
	if (ft_strcmp(s, "unset") == 0)
		return (1);
	if (ft_strcmp(s, "env") == 0)
		return (1);
	if (ft_strcmp(s, "exit") == 0)
		return (1);
	return (0);
}

static int	is_parent_only_builtin(const char *s)
{
	if (!s)
		return (0);
	if (ft_strcmp(s, "cd") == 0)
		return (1);
	if (ft_strcmp(s, "export") == 0)
		return (1);
	if (ft_strcmp(s, "unset") == 0)
		return (1);
	if (ft_strcmp(s, "exit") == 0)
		return (1);
	return (0);
}

int	execute_pipeline_entry(t_shell *sh)
{
	t_pipeline	*pl;
	t_command	*cmd;

	pl = sh->pipeline;
	if (!pl || pl->count == 0)
		return (0);

	cmd = &pl->cmds[0];

	// единственная команда и parent-only builtin -> выполнить в parent
	if (pl->count == 1 && cmd->argv && cmd->argv[0]
		&& is_parent_only_builtin(cmd->argv[0]))
	{
		return (run_builtin_in_parent_with_redirs(sh, cmd));
	}

	// иначе: fork pipeline; builtins в pipeline выполняются в child
	return (exec_pipeline_forking(sh, pl));
}

// И внутри child (в exec_pipeline_forking):
apply_redirs_or_die(cmd);

if (is_builtin(cmd->argv[0]))
	exit(run_builtin_child(sh, cmd));

exec_external(cmd, envp); // resolve + execve
