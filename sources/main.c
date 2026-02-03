#include "minishell.h"

int main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	int		exit_status;

	(void)argc;
	(void)argv;
	if (!init_shell(&shell, envp))
	{
		err_print(ES_GENERAL, "init_shell");
			return (EXIT_FAILURE);
	}
	setup_signals();
	minishell_loop(&shell);
	exit_status = shell.exit_status;
	shell_destroy(&shell);
	return (exit_status);
}